#include "PID.h"

#include "main.h"
#include "main_task.h"
#include "math.h"


void PID_Init()
{

}

float PID_Calc(PID * pid, float actual_val, float target_val)
{
  //计算误差
  pid->Error = target_val - actual_val;
  pid->pout = pid->Error;
  pid->iout += pid->Error;
  pid->dout = pid->Error - pid->LastError;

  //积分限幅
  if (pid->iout > pid->maxI) {
    pid->iout = pid->maxI;
  }
  else if (pid->iout < -pid->maxI) {
    pid->iout = -pid->maxI;
  }

  //计算输出
  pid->output = pid->kp * pid->pout + pid->ki * pid->iout + pid->kd * pid->dout;

  //误差传递
  pid->LastError = pid->Error;

  //输出限幅
  if (pid->output > pid->maxO) {
    pid->output = pid->maxO;
  }
  else if (pid->output < -pid->maxO) {
    pid->output = -pid->maxO;
  }
  return pid->output;
}
