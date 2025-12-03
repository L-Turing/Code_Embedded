#ifndef _PID_H
#define _PID_H
#pragma pack(push, 1)
#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"
#include "stdint.h"

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

    float output;
    int32_t Locked_Count;
    int32_t Locked_Judge;

  } PID;

  void PID_Init_Motor();
  float PID_Calc(PID* pid, float actual_val, float target_val);

#ifdef __cplusplus
}
#endif
#pragma pack(pop)
#endif
