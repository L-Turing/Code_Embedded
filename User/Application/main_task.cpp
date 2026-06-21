#include "main_task.h"

#include "IMU.h"
#include "Motor.h"
#include "PID.h"
#include "Peripheral.h"
#include "SDcard.h"
#include "SteeringGear.h"
#include "TIM.h"
#include "arm_math.h"
#include "bsp_can.h"
#include "bsp_dwt.h"
#include "bsp_usart.h"
#include "can.h"
#include "cmsis_os2.h"
#include "fatfs.h"
#include "iwdg.h"
#include "math.h"
#include "sdio.h"
#include "stdlib.h"
#include "string.h"
#include "usart.h"
#include "usbd_cdc_if.h"

extern "C" uint32_t running_time;

//变量-----------------------------------------------------------------------------
DJIMotor_Class motor_leftwheel(
  which_cans::can1, motor_types::M2006, 0x200, 0X201, type_signal::LED_1_ON);
DJIMotor_Class motor_rightwheel(
  which_cans::can1, motor_types::M2006, 0x200, 0X202, type_signal::LED_2_ON);
DJIMotor_Class motor_t(which_cans::can2, motor_types::M2006, 0x200, 0X202, type_signal::LED_3_ON);

Servo servo_yaw(&htim5, TIM_CHANNEL_4, -180, 180, 250, 1250);     //PI0
Servo servo_pitch1(&htim5, TIM_CHANNEL_3, -180, 180, 800, 1250);  //PH12
Servo servo_pitch2(&htim5, TIM_CHANNEL_2, -180, 180, 250, 1250);  //PH11
Servo servo_pitch3(&htim5, TIM_CHANNEL_1, -180, 180, 250, 1250);  //PH10
Servo servo_test(&htim4, TIM_CHANNEL_4, -180, 180, 250, 1250);    //PD15

static float wheel_speed[2] = {0.0f, 0.0f};  //电机输出轴转速 rpm
static float vx_temp = 0.0f, z_temp = 0.0f;  //临时速度变量 临时位置变量
static constexpr float nav_yaw_error_limit_deg = 15.0f;
static float nav_wz_temp = 0.0f;
//底盘运动学解算
PID wz_pid = {0.2f, 0.0f, 2.0f, 0, 0, 0, 0.2f, 3.0f, 0, 0, 0, 0, 0, 0};

//非任务函数-----------------------------------------------------------------------------

//处理按键状态
static void Key_Flag_Continue()
{
  flag.ps2_start_now = PS2.start;
  flag.ps2_select_now = PS2.select;
  flag.ps2_mode_now = PS2.mode;

  uint8_t * now_arr[3] = {&flag.ps2_start_now, &flag.ps2_select_now, &flag.ps2_mode_now};
  uint8_t * last_arr[3] = {&flag.ps2_start_last, &flag.ps2_select_last, &flag.ps2_mode_last};
  uint8_t * state_arr[3] = {&flag.ps2_start, &flag.ps2_select, &flag.ps2_mode};

  for (uint8_t i = 0; i < 3; i++) {  //0/1:状态切换
    if (*now_arr[i] > *last_arr[i]) {
      *state_arr[i] = (*state_arr[i] == 0) ? 1 : 0;
    }
    *last_arr[i] = *now_arr[i];
  }
}

//离线判断
static void OnlineJudge()
{
  PS2.ps2_online++;  //PS2离线计数
  if (PS2.ps2_online >= 500) {
    PS2.ps2_online = 500;
    NVIC_SystemReset();  //PS2离线则复位
  }
  else {
    //SignalMark(type_signal::Buzzer_OFF);
  }

  imu.imu_online++;  //IMU离线计数
  if (imu.imu_online >= 500) {
    imu.imu_online = 500;
  }
  else {
  }
}

static void Reset_Chassis_Yaw_Control()
{
  z_temp = imu.yaw_total;
  nav_wz_temp = 0.0f;

  wz_pid.pout = 0.0f;
  wz_pid.iout = 0.0f;
  wz_pid.dout = 0.0f;
  wz_pid.Error = 0.0f;
  wz_pid.LastError = 0.0f;
  wz_pid.PrevError = 0.0f;
  wz_pid.output = 0.0f;
}

static void Chassis_Solution(float vx, float vy, float wz_)  //m/s m/s 度
{
  static float wz = 0.0f;
  if (fabs(vx) > 3.0f || fabs(vy) > 3.0f) {  //异常值清零
    vx = vy = 0.0f;
  }
  wz = PID_Calc(&wz_pid, imu.yaw_total, wz_);

  if (imu.imu_online >= 500) wz = 0.0f;  //IMU离线时辅助速度清零

  wheel_speed[0] = (-vx - wz * Wheel_Base / 2.0f) / (Wheel_Radius * 2.0f * 3.1416f) * 60.0f;
  wheel_speed[1] = (+vx - wz * Wheel_Base / 2.0f) / (Wheel_Radius * 2.0f * 3.1416f) * 60.0f;

  for (uint8_t i = 0; i < 2; i++) {  //速度限幅
    if (wheel_speed[i] > Motor_Speed_Rpm_Out_Limit) {
      wheel_speed[i] = Motor_Speed_Rpm_Out_Limit;
    }
    else if (wheel_speed[i] < -Motor_Speed_Rpm_Out_Limit) {
      wheel_speed[i] = -Motor_Speed_Rpm_Out_Limit;
    }
  }
}

static void Chassis_Solution_Wz(float vx, float vy, float wz)  //m/s m/s rad/s
{
  if (fabs(vx) > 3.0f || fabs(vy) > 3.0f) {  //异常值清零
    vx = vy = 0.0f;
  }

  wheel_speed[0] = (-vx - 3.0f * wz * Wheel_Base / 2.0f) / (Wheel_Radius * 2.0f * 3.1416f) * 60.0f;
  wheel_speed[1] = (+vx - 3.0f * wz * Wheel_Base / 2.0f) / (Wheel_Radius * 2.0f * 3.1416f) * 60.0f;

  for (uint8_t i = 0; i < 2; i++) {  //速度限幅
    if (wheel_speed[i] > Motor_Speed_Rpm_Out_Limit) {
      wheel_speed[i] = Motor_Speed_Rpm_Out_Limit;
    }
    else if (wheel_speed[i] < -Motor_Speed_Rpm_Out_Limit) {
      wheel_speed[i] = -Motor_Speed_Rpm_Out_Limit;
    }
  }
}

//线性映射
double msp(double x, double in_min, double in_max, double out_min, double out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//任务函数-----------------------------------------------------------------------------
void StartChassis(void * argument)
{
  (void)argument;
  flag.ps2_start = 0;
  flag.ps2_mode = 0;
  uint8_t chassis_mode_last = flag.ps2_mode;
  for (;;) {
    //更新电机状态
    motor_leftwheel.Update_Status(data_can_receive[0]);
    motor_rightwheel.Update_Status(data_can_receive[1]);
    motor_t.Update_Status(data_can_receive[2]);

    if (flag.ps2_mode != chassis_mode_last) {
      Reset_Chassis_Yaw_Control();
      chassis_mode_last = flag.ps2_mode;
    }

    if (flag.ps2_start == 0) {  //未启动时，清零平移速度并保持当前航向
      vx_temp = 0.0f;
      Reset_Chassis_Yaw_Control();
      SignalMark(type_signal::LED_R_ON);
      SignalMark(type_signal::LED_G_OFF);
    }
    else {  //启动后按模式控制
      SignalMark(type_signal::LED_G_ON);
      SignalMark(type_signal::LED_R_OFF);
      switch (flag.ps2_mode) {
        case 0:  //手动
          if (PS2.right_x[0] && !PS2.right_x[1])
            vx_temp = 0.3f;
          else if (!PS2.right_x[0] && PS2.right_x[1])
            vx_temp = -0.3f;
          else
            vx_temp = 0.0f;

          nav_wz_temp = 0.0f;
          if (PS2.right_y[0] && !PS2.right_y[1])
            z_temp += 0.1f;
          else if (!PS2.right_y[0] && PS2.right_y[1])
            z_temp -= 0.1f;

          break;

        case 1:  //导航
          if (fabsf(r_packet.v) > 0.02f) {
            vx_temp = r_packet.v;
          }
          else {
            vx_temp = 0.0f;
          }

          if (fabsf(r_packet.yaw_tar_rad_s) > 0.02f) {
            nav_wz_temp = r_packet.yaw_tar_rad_s;
          }
          else {
            nav_wz_temp = 0.0f;
          }

          break;

        default:
          vx_temp = 0.0f;
          Reset_Chassis_Yaw_Control();
          break;
      }
    }

    if (flag.ps2_start != 0 && flag.ps2_mode == 1)
      Chassis_Solution_Wz(vx_temp, 0, nav_wz_temp);
    else
      Chassis_Solution(vx_temp, 0, z_temp);
    motor_leftwheel.SetWheel(wheel_speed[0]);
    motor_rightwheel.SetWheel(wheel_speed[1]);

    CanSend(
      1, DJI, 0x200, (int16_t)(motor_leftwheel.output), (int16_t)(motor_rightwheel.output), 0, 0);
    HAL_IWDG_Refresh(&hiwdg);
    osDelay(2);
  }
}

void StartRemote(void * argument)
{
  (void)argument;
  imu_init(0X04, 0X14, &hcan2);  //初始化IMU
  for (;;) {
    //处理按键状态，离线判断
    Key_Flag_Continue();
    OnlineJudge();

    //请求IMU数据
    imu_request_euler();
    osDelay(1);
    imu_request_accel();
    osDelay(1);

    if (imu.yaw - imu.yaw_last > 180.0f) imu.yaw_cirnum--;
    if (imu.yaw - imu.yaw_last < -180.0f) imu.yaw_cirnum++;
    imu.yaw_total = imu.yaw + imu.yaw_cirnum * 360.0f;
    imu.yaw_last = imu.yaw;

    if (flag.ps2_start != 0) {
      if ((running_time % 200U) < 100U)
        motor_t.SetWheel(-50.0f);
      else
        motor_t.SetWheel(0.0f);
    }
    else
      motor_t.SetWheel(0.0f);

    CanSend(
      2, DJI, 0x200, (int16_t)(motor_t.output), (int16_t)(motor_t.output),
      (int16_t)(motor_t.output), (int16_t)(motor_t.output));
    HAL_IWDG_Refresh(&hiwdg);
    osDelay(1);
  }
}

void StartServo(void * argument)
{
  (void)argument;
  for (;;) {
    HAL_IWDG_Refresh(&hiwdg);
    osDelay(2);
  }
}

void StartAnalogLower(void * argument)
{
  (void)argument;
  for (;;) {
    HAL_IWDG_Refresh(&hiwdg);
    osDelay(100);
  }
}

void StartAnalogUpper(void * argument)
{
  (void)argument;
  for (;;) {
    HAL_IWDG_Refresh(&hiwdg);
    osDelay(100);
  }
}
