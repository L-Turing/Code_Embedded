#include "main_task.h"
#include "DJIMotor.h"
#include "IMU.h"
#include "PID.h"
#include "Peripheral.h"
#include "arm_math.h"
#include "bsp_can.h"
#include "bsp_dwt.h"
#include "bsp_usart.h"
#include "can.h"
#include "cmsis_os.h"
#include "iwdg.h"
#include "math.h"
#include "stdlib.h"
#include "string.h"

static float vx_remote = 0.0f, vy_remote = 0.0f, wz_remote = 0.0f;
static float vx = 0.0f, vy = 0.0f, wz = 0.0f;
static float v_tar[4] = {0, 0, 0, 0};
static float v_stop[4] = {0, 0, 0, 0};
static void Chassis_Solution(RC_Ctl_t RC_Ctl_temp, float * wheel_speed);

//任务函数------
uint8_t last_s2 = 0;
float target_angle = 0.0f;
void StartChassis(void * argument)
{
  (void)argument;
  for (;;) {
    Chassis_Solution(rc_ctl, v_tar);
    if (rc_ctl.s1 == 2 || rc_ctl.s2 == 2) {
      CanSend_DJIMotor(2, 0x200, v_stop[0], v_stop[1], v_stop[2], v_stop[3]);
      target_angle = motor_2006.accumlate_rad_out * 57.583f;  //记录当前角度
      CanSend_DJIMotor(1, 0x200, v_stop[0], v_stop[1], v_stop[2], v_stop[3]);
    }
    else {
      Chassis_Motor(2, v_tar);

      if ((rc_ctl.s2 == 1) && (last_s2 == 3)) {
        target_angle += (float)(36.0f * 1.0f);
      }
      //Set2006(800, Speed_Mode);
    }
    last_s2 = rc_ctl.s2;
    Update_Info_DJIMotor(M3508_motor, 4);    //底盘电机
    Update_Info_DJIMotor(&GM6020_motor, 1);  //yaw轴电机
    Update_Info_DJIMotor(&motor_2006, 1);    //云台电机
    HAL_IWDG_Refresh(&hiwdg);
    osDelay(2);
  }
}

void StartRemote(void * argument)
{
  (void)argument;
  for (;;) {
    USART1_RemoteCallback();  //DT7
    USART7_RemoteCallback();  //裁判系统
    IMU_RequestData(&hcan2, 0x03, 0x01);
    osDelay(1);
    IMU_RequestData(&hcan2, 0x03, 0x02);
    osDelay(1);
    IMU_RequestData(&hcan2, 0x03, 0x03);
    osDelay(1);
    IMU_RequestData(&hcan2, 0x03, 0x04);
    osDelay(1);
    // IMU_RequestData(&hcan2, 0x03, 0x02);
    osDelay(1);
  }
}

extern uint32_t running_time_10ms;
void StartUI(void * argument)
{
  (void)argument;
  for (;;) {
    // UI相关代码  显示对方血量，小陀螺，自瞄（装甲板/打符），摩擦轮，放大允许发弹量，实体弹倒计数
    osDelay(2);
  }
}
//------任务函数

static float wheel_diameter = 0.195f;             //全向轮直径
static float spin_diameter = 0.42f;               //车体自旋直径
static float Motor_Speed_Rpm_Out_Limit = 450.0f;  //电机rpm转速限制
static void Chassis_Solution(RC_Ctl_t RC_Ctl_temp, float * wheel_speed)
{
  //异常值清零
  if (
    (abs(RC_Ctl_temp.ch0) > 661) || (abs(RC_Ctl_temp.ch1) > 661) || (abs(RC_Ctl_temp.ch2) > 661) ||
    (abs(RC_Ctl_temp.ch3) > 661) || (abs(RC_Ctl_temp.ch4) > 661)) {
    memset(&RC_Ctl_temp, 0, sizeof(RC_Ctl_t));
  }

  //映射为 m/s
  vx_remote = (float)RC_Ctl_temp.ch3 / 130.0f * 0.7071f;  //660.0f /130.0f * 0.7071f = 3.600f m/s
  vy_remote = (float)RC_Ctl_temp.ch2 / 130.0f * 0.7071f;

  //底盘最小回归角
  float ecd_angle_out_tmp = 0.0f;
  float wz_reg = 0.0f;
  if (GM6020_motor.ecd_angle_out - D_YAW_BASE_FIRST < -180.0f) {
    ecd_angle_out_tmp = GM6020_motor.ecd_angle_out + 360.0f;
  }
  else if (GM6020_motor.ecd_angle_out - D_YAW_BASE_FIRST > +180.0f) {
    ecd_angle_out_tmp = GM6020_motor.ecd_angle_out - 360.0f;
  }
  else {
    ecd_angle_out_tmp = GM6020_motor.ecd_angle_out;
  }
  if (fabs(ecd_angle_out_tmp - D_YAW_BASE_FIRST) > 15.0f) {  //±15°之外或±15°之内且移动 则回归
    wz_reg = PID_Calc(&wz_pid, ecd_angle_out_tmp, D_YAW_BASE_FIRST);  //底盘回归角速度 m/s
  }
  else {            //±15°之内,且不移动 则不回归
    wz_reg = 0.0f;  //底盘回归角速度 m/s
  }

  //小陀螺
  if (RC_Ctl_temp.ch4 > 100) {
    wz_remote = wz_reg + spin_diameter * PI * 2.0f;  // m/s  2.0f(圈每秒)
  }
  else if (RC_Ctl_temp.ch4 < -100) {
    wz_remote = wz_reg - spin_diameter * PI * 2.0f;
    ;
  }
  else {
    wz_remote = wz_reg;
  }
  //底盘回归角及小陀螺下移动的角速度补偿
  float follow_angle =
    (ecd_angle_out_tmp - D_YAW_BASE_FIRST) / 180.0f * PI + 0.003f * wz_remote * 2 * PI;  //弧度 rad

  //底盘跟随
  vx = vx_remote * cos(follow_angle) + vy_remote * sin(follow_angle);
  vy = vy_remote * cos(follow_angle) - vx_remote * sin(follow_angle);
  wz = wz_remote;

  //单位转换
  float scale = 1 / (3.14159f * wheel_diameter) * 60.0f;  //输出轴 rpm

  //底盘解算
  wheel_speed[0] = scale * (-vy + vx + wz);
  wheel_speed[1] = scale * (+vy + vx + wz);
  wheel_speed[2] = scale * (+vy - vx + wz);
  wheel_speed[3] = scale * (-vy - vx + wz);

  //速度限幅
  for (uint8_t i = 0; i < 4; i++) {
    if (wheel_speed[i] > Motor_Speed_Rpm_Out_Limit) {
      wheel_speed[i] = Motor_Speed_Rpm_Out_Limit;
    }
    else if (wheel_speed[i] < -Motor_Speed_Rpm_Out_Limit) {
      wheel_speed[i] = -Motor_Speed_Rpm_Out_Limit;
    }
  }
}

double msp(double x, double in_min, double in_max, double out_min, double out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
