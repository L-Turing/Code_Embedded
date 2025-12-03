#include "main_task.h"

#include "Motor.h"
#include "Peripheral.h"
#include "SteeringGear.h"
#include "TIM.h"
#include "arm_math.h"
#include "bsp_can.h"
#include "bsp_dwt.h"
#include "bsp_usart.h"
#include "can.h"
#include "cmsis_os2.h"
#include "iwdg.h"
#include "math.h"
#include "stdlib.h"
#include "string.h"

//变量
DJIMotor_Class motor_leftwheel(
  which_cans::can1, motor_types::M2006, 0X200, 0X201, type_signal::LED_1_ON);
DJIMotor_Class motor_rightwheel(
  which_cans::can1, motor_types::M2006, 0X200, 0X202, type_signal::LED_2_ON);
Servo RDS3218_test(&htim5, TIM_CHANNEL_4, 0, 270, 500, 2500);

static float wheel_speed[2] = {0, 0};         //电机输出轴转速 rpm
static float vx_temp = 0.0f, wz_temp = 0.0f;  //临时速度变量

//非任务函数
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

static void Chassis_Solution(float vx, float vy, float wz)  //m/s m/s rad/s
{
  if (fabs(vx) > 2.0f || fabs(vy) > 2.0f || fabs(wz) > 2 * PI) {  //异常值清零
    vx = vy = wz = 0.0f;
  }

  wheel_speed[0] = (+vx + wz * Wheel_Base / 2.0f) / Wheel_Radius / (2.0f * 3.1416f) * 60.0f;
  wheel_speed[1] = (-vx + wz * Wheel_Base / 2.0f) / Wheel_Radius / (2.0f * 3.1416f) * 60.0f;

  for (uint8_t i = 0; i < 2; i++) {  //速度限幅
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

//任务函数
void StartChassis(void * argument)
{
  (void)argument;
  for (;;) {
    motor_leftwheel.Update_Status(data_can_receive[0]);
    motor_rightwheel.Update_Status(data_can_receive[1]);

    if (flag.ps2_select == 1 && flag.ps2_mode == 0) {                 //遥控
      vx_temp = msp(PS2.right_joystick_x, 0, 255, -1.0, +1.0);        //m/s
      wz_temp = msp(PS2.right_joystick_y, 0, 255, -PI / 2, +PI / 2);  //rad/s
      Chassis_Solution(vx_temp, 0, wz_temp);

      motor_leftwheel.SetWheel(wheel_speed[0]);
      motor_rightwheel.SetWheel(wheel_speed[1]);

      SignalMark(type_signal::LED_R_ON);
    }
    else if (flag.ps2_select == 1 && flag.ps2_mode == 1) {  //导航
      SignalMark(type_signal::LED_R_Breathe_ON);
    }
    else {  //急停
      motor_leftwheel.output = 0.0f;
      motor_rightwheel.output = 0.0f;

      SignalMark(type_signal::LED_R_OFF);
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
  for (;;) {
    Key_Flag_Continue();

    HAL_IWDG_Refresh(&hiwdg);
    osDelay(2);
  }
}

void StartOthers(void * argument)
{
  (void)argument;
  for (;;) {
    float angle_temp = msp(PS2.left_joystick_x, 0, 255, 0, 270);
    RDS3218_test.Servo_Control((uint16_t)angle_temp);
    HAL_IWDG_Refresh(&hiwdg);
    osDelay(5);
  }
}