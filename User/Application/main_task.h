#ifndef __MAIN_TASK_H
#define __MAIN_TASK_H

#include "Motor.h"
#include "SteeringGear.h"
#include "stdint.h"

#define PI 3.14159265f
#define Motor_Speed_Rpm_Out_Limit 400.0f  //电机输出轴限速 rpm
#define Wheel_Base 0.29f                  //差速轮间距m
#define Wheel_Radius 0.03375f             //轮半径m

#pragma pack(push, 1)
#ifdef __cplusplus
extern "C" {
#endif
typedef struct
{
  float analog_yaw;
  uint8_t analog_step;
  float analog_step_rad;  //转动步数对应的轮子转过的弧度
  uint8_t flag;
} Analog_Packet;

void StartChassis(void * argument);
void StartRemote(void * argument);
void StartServo(void * argument);
void StartAnalogUpper(void * argument);
void StartAnalogLower(void * argument);
double msp(double x, double in_min, double in_max, double out_min, double out_max);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern DJIMotor_Class motor_leftwheel;
extern DJIMotor_Class motor_rightwheel;
extern Servo servo_yaw;
extern Servo servo_pitch1;
extern Servo servo_pitch2;
extern Servo servo_pitch3;
extern Servo servo_test;

#endif

#pragma pack(pop)

#endif
