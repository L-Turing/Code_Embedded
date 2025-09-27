#ifndef __PID_H
#define __PID_H

#include "main.h"
#include "stdint.h"

#pragma pack(push, 1)

#ifdef __cplusplus
extern "C" {
#endif

// PID控制器结构体定义
// 包含比例系数、积分系数、微分系数、输出限幅、误差、上次误差、输出值等信息
// 该结构体用于实现PID控制算法，计算目标值与实际值之间的误差，并根据PID算法调整输出值
typedef struct
{
  float kp;
  float ki;
  float kd;

  float pout;
  float iout;
  float dout;

  float maxI;  //maxI积分限幅
  float maxO;  //maxO输出限幅

  float Error;
  float LastError;
  float PrevError;

  float output_val;
} PID;

extern PID wz_pid;
void PID_Init();
float PID_Calc(PID * pid, float actual_val, float target_val);

#ifdef __cplusplus
}
#endif

#pragma pack(pop)

#endif
