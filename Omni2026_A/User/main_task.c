#include "main_task.h"

#include "DJIMotor.h"
#include "Peripheral.h"
#include "bsp_can.h"
#include "bsp_dwt.h"
#include "bsp_usart.h"
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
void StartChassis(void * argument)
{
  (void)argument;
  for (;;) {
    Chassis_Solution(rc_ctl, v_tar);
    if (rc_ctl.s1 == 2 && rc_ctl.s2 == 2) {
      CanSend_DJIMotor(0, 0x200, v_stop[0], v_stop[1], v_stop[2], v_stop[3]);
    }
    else {
      Chassis_Motor(1, v_tar);
    }
    Update_Info_DJIMotor(M3508_motor, 4);
    HAL_IWDG_Refresh(&hiwdg);
    osDelay(2);
  }
}

void StartRemote(void * argument)
{
  (void)argument;
  for (;;) {
    USART1_RemoteCallback();
    USART7_RemoteCallback();
    osDelay(5);
  }
}

void StartUI(void * argument)
{
  (void)argument;
  for (;;) {
    // UI相关代码
    osDelay(10);
  }
}
//------任务函数

static float wheel_diameter = 0.195f;
static float spin_diameter = 0.42f;
static float Motor_Speed_Rpm_Out_Limit = 400.0f;  //电机速度限制
static void Chassis_Solution(RC_Ctl_t RC_Ctl_temp, float * wheel_speed)
{
  if (
    (abs(RC_Ctl_temp.ch0) > 661) || (abs(RC_Ctl_temp.ch1) > 661) || (abs(RC_Ctl_temp.ch2) > 661) ||
    (abs(RC_Ctl_temp.ch3) > 661) || (abs(RC_Ctl_temp.ch4) > 661)) {
    memset(&RC_Ctl_temp, 0, sizeof(RC_Ctl_t));
  }

  //基本速度
  vx_remote = (float)RC_Ctl_temp.ch3 / 130.0f * 0.7071f;  //660.0f /130.0f * 0.7071f = 3.600f m/s
  vy_remote = (float)RC_Ctl_temp.ch2 / 130.0f * 0.7071f;
  //float follow_angle = (float)(D_yaw.pos - D_YAW_BASE_FIRST); //rad

  //wz处理
  if (RC_Ctl_temp.ch4 > 100 || RC_Ctl_temp.ch4 < -100) {
    wz_remote = spin_diameter * 3.14159f * 2.5f;  // m/s
  }
  else {
    wz_remote=0;
    //wz_remote = -PID_Calc(&wz_pid,D_yaw.pos*57.29578f,D_YAW_BASE_FIRST*57.29578f);
  }

  //底盘跟随
  vx = vx_remote;
  vy = vy_remote;
  wz = wz_remote;
  //vx = vx_remote * cos(follow_angle) + vy_remote * sin(follow_angle);
  //vy = vy_remote * cos(follow_angle) - vx_remote * sin(follow_angle);
  //wz = wz_remote;

  float scale = 1 / (3.14159f * wheel_diameter) * 60.0f;  //输出轴 rpm

  wheel_speed[0] = scale * (-vy + vx + wz);
  wheel_speed[1] = scale * (+vy + vx + wz);
  wheel_speed[2] = scale * (+vy - vx + wz);
  wheel_speed[3] = scale * (-vy - vx + wz);
  
  for(uint8_t i = 0; i < 4; i++) {
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
