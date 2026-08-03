// #include "main_task.h"

// #include "IMU.h"
// #include "Motor.h"
// #include "PID.h"
// #include "Peripheral.h"
// #include "SDcard.h"
// #include "SteeringGear.h"
// #include "TIM.h"
// #include "arm_math.h"
// #include "bsp_dwt.h"
// #include "bsp_usart.h"
// #include "buzzer.h"
// #include "cmsis_os2.h"
// #include "fatfs.h"
// #include "iwdg.h"
// #include "math.h"
// #include "sdio.h"
// #include "stdlib.h"
// #include "string.h"
// #include "usart.h"
// #include "usbd_cdc_if.h"

// extern "C" uint32_t running_time;

// //变量-----------------------------------------------------------------------------
// // CAN 已移除，电机对象暂不可用
// // (原 DJIMotor_Class 声明已移除)

// Servo servo_yaw(&htim5, TIM_CHANNEL_4, -180, 180, 250, 1250);     //PI0
// Servo servo_pitch1(&htim5, TIM_CHANNEL_3, -180, 180, 800, 1250);  //PH12
// Servo servo_pitch2(&htim5, TIM_CHANNEL_2, -180, 180, 250, 1250);  //PH11
// Servo servo_pitch3(&htim5, TIM_CHANNEL_1, -180, 180, 250, 1250);  //PH10
// Servo servo_test(&htim4, TIM_CHANNEL_4, -180, 180, 250, 1250);    //PD15

// static float wheel_speed[2] = {0.0f, 0.0f};  //电机输出轴转速 rpm
// static float vx_temp = 0.0f, z_temp = 0.0f;  //临时速度变量 临时位置变量
// static constexpr float nav_yaw_error_limit_deg = 15.0f;
// static float nav_wz_temp = 0.0f;
// //底盘运动学解算
// PID wz_pid = {0.2f, 0.0f, 2.0f, 0, 0, 0, 0.2f, 3.0f, 0, 0, 0, 0, 0, 0};

// //非任务函数-----------------------------------------------------------------------------

// //处理按键状态
// static void Key_Flag_Continue()
// {
//   flag.ps2_start_now = PS2.start;
//   flag.ps2_select_now = PS2.select;
//   flag.ps2_mode_now = PS2.mode;

//   uint8_t * now_arr[3] = {&flag.ps2_start_now, &flag.ps2_select_now, &flag.ps2_mode_now};
//   uint8_t * last_arr[3] = {&flag.ps2_start_last, &flag.ps2_select_last, &flag.ps2_mode_last};
//   uint8_t * state_arr[3] = {&flag.ps2_start, &flag.ps2_select, &flag.ps2_mode};

//   for (uint8_t i = 0; i < 3; i++) {  //0/1:状态切换
//     if (*now_arr[i] > *last_arr[i]) {
//       *state_arr[i] = (*state_arr[i] == 0) ? 1 : 0;
//     }
//     *last_arr[i] = *now_arr[i];
//   }
// }

// //离线判断
// static void OnlineJudge()
// {
//   PS2.ps2_online++;  //PS2离线计数
//   if (PS2.ps2_online >= 500) {
//     PS2.ps2_online = 500;
//     NVIC_SystemReset();  //PS2离线则复位
//   }
//   else {
//     //SignalMark(type_signal::Buzzer_OFF);
//   }

//   imu.imu_online++;  //IMU离线计数
//   if (imu.imu_online >= 500) {
//     imu.imu_online = 500;
//   }
//   else {
//   }
// }

// static void Reset_Chassis_Yaw_Control()
// {
//   z_temp = imu.yaw_total;
//   nav_wz_temp = 0.0f;

//   wz_pid.pout = 0.0f;
//   wz_pid.iout = 0.0f;
//   wz_pid.dout = 0.0f;
//   wz_pid.Error = 0.0f;
//   wz_pid.LastError = 0.0f;
//   wz_pid.PrevError = 0.0f;
//   wz_pid.output = 0.0f;
// }

// static void Chassis_Solution(float vx, float vy, float wz_)  //m/s m/s 度
// {
//   static float wz = 0.0f;
//   if (fabs(vx) > 3.0f || fabs(vy) > 3.0f) {  //异常值清零
//     vx = vy = 0.0f;
//   }
//   wz = PID_Calc(&wz_pid, imu.yaw_total, wz_);

//   if (imu.imu_online >= 500) wz = 0.0f;  //IMU离线时辅助速度清零

//   wheel_speed[0] = (-vx - wz * Wheel_Base / 2.0f) / (Wheel_Radius * 2.0f * 3.1416f) * 60.0f;
//   wheel_speed[1] = (+vx - wz * Wheel_Base / 2.0f) / (Wheel_Radius * 2.0f * 3.1416f) * 60.0f;

//   for (uint8_t i = 0; i < 2; i++) {  //速度限幅
//     if (wheel_speed[i] > Motor_Speed_Rpm_Out_Limit) {
//       wheel_speed[i] = Motor_Speed_Rpm_Out_Limit;
//     }
//     else if (wheel_speed[i] < -Motor_Speed_Rpm_Out_Limit) {
//       wheel_speed[i] = -Motor_Speed_Rpm_Out_Limit;
//     }
//   }
// }

// static void Chassis_Solution_Wz(float vx, float vy, float wz)  //m/s m/s rad/s
// {
//   if (fabs(vx) > 3.0f || fabs(vy) > 3.0f) {  //异常值清零
//     vx = vy = 0.0f;
//   }

//   wheel_speed[0] = (-vx - 3.0f * wz * Wheel_Base / 2.0f) / (Wheel_Radius * 2.0f * 3.1416f) * 60.0f;
//   wheel_speed[1] = (+vx - 3.0f * wz * Wheel_Base / 2.0f) / (Wheel_Radius * 2.0f * 3.1416f) * 60.0f;

//   for (uint8_t i = 0; i < 2; i++) {  //速度限幅
//     if (wheel_speed[i] > Motor_Speed_Rpm_Out_Limit) {
//       wheel_speed[i] = Motor_Speed_Rpm_Out_Limit;
//     }
//     else if (wheel_speed[i] < -Motor_Speed_Rpm_Out_Limit) {
//       wheel_speed[i] = -Motor_Speed_Rpm_Out_Limit;
//     }
//   }
// }

// //线性映射
// double msp(double x, double in_min, double in_max, double out_min, double out_max)
// {
//   return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
// }

// //任务函数-----------------------------------------------------------------------------
// void StartChassis(void * argument)
// {
//   (void)argument;
//   for (;;) {
//     //HAL_IWDG_Refresh(&hiwdg);
//     osDelay(2);
//   }
// }

// void StartRemote(void * argument)
// {
//   (void)argument;
//   for (;;) {
//     //HAL_IWDG_Refresh(&hiwdg);
//     osDelay(1);
//   }
// }

// void StartServo(void * argument)
// {
//   (void)argument;
//   for (;;) {
//     // HAL_IWDG_Refresh(&hiwdg);
//     osDelay(2);
//   }
// }

// void StartAnalogLower(void * argument)
// {
//   (void)argument;
//   for (;;) {
//     gala_you();
//     //HAL_IWDG_Refresh(&hiwdg);
//     osDelay(1000);
//   }
// }

// void StartAnalogUpper(void * argument)
// {
//   (void)argument;
//   for (;;) {
//     //HAL_IWDG_Refresh(&hiwdg);
//     osDelay(100);
//   }
// }
