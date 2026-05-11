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

Analog_Packet a_packet[22];  //模拟量数据包
uint8_t i_a = 0;             //模拟量数据包计数

//变量-----------------------------------------------------------------------------
DJIMotor_Class motor_leftwheel(
  which_cans::can1, motor_types::M2006, 0x200, 0X201, type_signal::LED_1_ON);
DJIMotor_Class motor_rightwheel(
  which_cans::can1, motor_types::M2006, 0x200, 0X202, type_signal::LED_2_ON);
Servo servo_yaw(&htim5, TIM_CHANNEL_4, -180, 180, 250, 1250);     //PI0
Servo servo_pitch1(&htim5, TIM_CHANNEL_3, -180, 180, 800, 1250);  //PH12
Servo servo_pitch2(&htim5, TIM_CHANNEL_2, -180, 180, 250, 1250);  //PH11
Servo servo_pitch3(&htim5, TIM_CHANNEL_1, -180, 180, 250, 1250);  //PH10
Servo servo_test(&htim4, TIM_CHANNEL_4, -180, 180, 250, 1250);    //PD15

static float wheel_speed[2] = {0.0f, 0.0f};  //电机输出轴转速 rpm
static float vx_temp = 0.0f, z_temp = 0.0f;  //临时速度变量 临时位置变量
//底盘运动学解算
PID wz_pid = {0.4f, 0.002f, 0.0f, 0, 0, 0, 0.16f, 3.0f, 0, 0, 0, 0, 0, 0};

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
    //SignalMark(type_signal::Buzzer_ON);
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

static void Chassis_Solution(float vx, float vy, float wz_)  //m/s m/s 度
{
  static float wz = 0.0f;
  if (fabs(vx) > 3.0f || fabs(vy) > 3.0f) {  //异常值清零
    vx = vy = 0.0f;
  }

  wz = PID_Calc(&wz_pid, imu.yaw_total, wz_);  //根据角度误差计算辅助角速度
  if (imu.imu_online >= 500) wz = 0.0f;        //IMU离线时辅助速度清零

  // wz = 0.0f;  //暂时不使用陀螺仪反馈，直接用遥控器输入的角速度
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

//线性映射
double msp(double x, double in_min, double in_max, double out_min, double out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//任务函数-----------------------------------------------------------------------------
void StartChassis(void * argument)
{
  (void)argument;
  for (;;) {
    //更新电机状态
    motor_leftwheel.Update_Status(data_can_receive[0]);
    motor_rightwheel.Update_Status(data_can_receive[1]);

    if (flag.ps2_start == 0) {  //遥控器开始键未按下，底盘不动
      motor_leftwheel.output = 0.0f;
      motor_rightwheel.output = 0.0f;

      SignalMark(type_signal::LED_R_OFF);
    }
    else {  //遥控器开始键按下，底盘根据右侧按键控制运动
      switch (flag.ps2_mode) {
        case 0:  //手动
          if (PS2.right_x[0] && !PS2.right_x[1])
            vx_temp = 0.5f;
          else if (!PS2.right_x[0] && PS2.right_x[1])
            vx_temp = -0.5f;
          else
            vx_temp = 0.0f;

          if (PS2.right_y[0] && !PS2.right_y[1])
            z_temp += 0.1f;
          else if (!PS2.right_y[0] && PS2.right_y[1])
            z_temp -= 0.1f;

          SignalMark(type_signal::LED_R_ON);
          break;

        case 1:  //导航
                 // if (r_packet.v > 0.1f)
                 //   vx_temp = 0.5f;
                 // else if (r_packet.v < -0.1f)
                 //   vx_temp = -0.5f;
                 // else
                 //   vx_temp = 0.0f;
                 // if (r_packet.yaw > 0.1f)
                 //   z_temp += 0.1f;
                 // else if (r_packet.yaw < -0.1f)
                 //   z_temp -= 0.1f;

          //模拟量测试
          //转动部分因为改变了z_temp的值，此处不用管，下方底盘解算会自动执行
          //位置部分根据a_packet的flag状态机执行，flag=0未开始，flag=1执行转动，flag=2转动完成执行位置，flag=3位置完成
          if (a_packet[i_a].flag == 2)  //角度完成，执行位置控制
            vx_temp = 0.5f;             //固定速度
          else
            vx_temp = 0.0f;

          SignalMark(type_signal::LED_R_Breathe_ON);
          break;

        default:
          break;
      }

      Chassis_Solution(vx_temp, 0, z_temp);
      motor_leftwheel.SetWheel(wheel_speed[0]);
      motor_rightwheel.SetWheel(wheel_speed[1]);
    }

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

    HAL_IWDG_Refresh(&hiwdg);
    osDelay(2);
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
    if (flag.ps2_start == 0 && flag.ps2_mode == 1) {  //导航模式且未开始，执行模拟量测试
      if (a_packet[i_a].flag > 100) return;           //终点

      if (a_packet[i_a].flag == 0) {
        a_packet[i_a].flag = 1;              //开始执行
        z_temp += a_packet[i_a].analog_yaw;  //更新目标角度
        a_packet[i_a].analog_step_rad =
          motor_leftwheel.accumlate_rad_out + a_packet[i_a].analog_step_rad;
        a_packet[i_a].analog_step_rad =
          motor_leftwheel.accumlate_rad_out - a_packet[i_a].analog_step_rad;
      }

      if (fabs(imu.yaw_total - z_temp) < 2.0f && a_packet[i_a].flag == 1)  //接近目标角度
        a_packet[i_a].flag = 2;                                            //角度完成

      if (
        fabs(motor_leftwheel.accumlate_rad_out - a_packet[i_a].analog_step_rad) < 0.26f &&
        fabs(motor_rightwheel.accumlate_rad_out - a_packet[i_a].analog_step_rad) < 0.26f &&
        a_packet[i_a].flag == 2)  //接近目标位置且转动已完成 15度误差约0.26弧度
        a_packet[i_a].flag = 3;

      if (a_packet[i_a].flag == 3) {  //位置完成
        i_a++;
        osDelay(1500);  //完成一个数据包后等待1.5秒再执行下一个
      }
    }

    HAL_IWDG_Refresh(&hiwdg);
    osDelay(2);
  }
}

void StartAnalogUpper(void * argument)
{
  (void)argument;
  a_packet[0] = {-90, 2, 2.0 / Wheel_Radius, 0};
  a_packet[1] = {+90, 1, 1.0 / Wheel_Radius, 0};
  a_packet[2] = {+90, 2, 2.0 / Wheel_Radius, 0};
  a_packet[3] = {-90, 1, 1.0 / Wheel_Radius, 0};
  a_packet[4] = {-90, 6, 6.0 / Wheel_Radius, 0};
  a_packet[5] = {-90, 2, 2.0 / Wheel_Radius, 0};
  a_packet[6] = {-90, 3, 3.0 / Wheel_Radius, 0};
  a_packet[7] = {+180, 3, 3.0 / Wheel_Radius, 0};
  a_packet[8] = {+90, 2, 2.0 / Wheel_Radius, 0};
  a_packet[9] = {+90, 2, 2.0 / Wheel_Radius, 0};
  a_packet[10] = {-90, 1, 1.0 / Wheel_Radius, 0};
  a_packet[11] = {+90, 1, 1.0 / Wheel_Radius, 0};
  a_packet[12] = {-90, 1, 1.0 / Wheel_Radius, 0};
  a_packet[13] = {+90, 3, 3.0 / Wheel_Radius, 0};
  a_packet[14] = {-90, 1, 1.0 / Wheel_Radius, 0};
  a_packet[15] = {-90, 6, 6.0 / Wheel_Radius, 0};
  a_packet[16] = {-90, 1, 1.0 / Wheel_Radius, 0};
  a_packet[17] = {-90, 3, 3.0 / Wheel_Radius, 0};
  a_packet[18] = {+90, 2, 2.0 / Wheel_Radius, 0};
  a_packet[19] = {-90, 3, 3.0 / Wheel_Radius, 0};
  a_packet[20] = {+90, 2, 2.0 / Wheel_Radius, 0};  //
  a_packet[21] = {+200, +200, +200, +200};         //终值测试

  for (;;) {
    HAL_IWDG_Refresh(&hiwdg);
    osDelay(100);
  }
}
