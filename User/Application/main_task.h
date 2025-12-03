#ifndef __MAIN_TASK_H
#define __MAIN_TASK_H

#include "Motor.h"
#include "stdint.h"

#define PI 3.14159265f
#define Motor_Speed_Rpm_Out_Limit 400.0f  //电机输出轴限速 rpm
#define Wheel_Base 0.5f                   //差速轮间距m
#define Wheel_Radius 0.04f                //轮半径m

#pragma pack(push, 1)
#ifdef __cplusplus
extern "C" {
#endif
void StartChassis(void * argument);
void StartRemote(void * argument);
void StartOthers(void * argument);
double msp(double x, double in_min, double in_max, double out_min, double out_max);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern DJIMotor_Class motor_leftwheel;
extern DJIMotor_Class motor_rightwheel;
#endif

#pragma pack(pop)

#endif
