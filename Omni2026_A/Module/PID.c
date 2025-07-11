#include "PID.h"

#include "main.h"
#include "math.h"

PID M3508_pid[4] = {
  {50.0f, 1.0f, 0.0f, 0, 0, 0, 2000, 12000, 0, 0, 0, 0},
  {50.0f, 1.0f, 0.0f, 0, 0, 0, 2000, 12000, 0, 0, 0, 0},
  {50.0f, 1.0f, 0.0f, 0, 0, 0, 2000, 12000, 0, 0, 0, 0},
  {50.0f, 1.0f, 0.0f, 0, 0, 0, 2000, 12000, 0, 0, 0, 0},
};
PID wz_pid = {2.0f, 0.2f, 0.0f, 0, 0, 0, 1.0, 10, 0, 0, 0, 0};

// PID计算函数
// 输入实际值和目标值，返回PID输出值
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
  pid->output_val = pid->kp * pid->pout + pid->ki * pid->iout + pid->kd * pid->dout;

  //误差传递
  pid->LastError = pid->Error;

  //输出限幅
  if (pid->output_val > pid->maxO) {
    pid->output_val = pid->maxO;
  }
  else if (pid->output_val < -pid->maxO) {
    pid->output_val = -pid->maxO;
  }

  return pid->output_val;
}
