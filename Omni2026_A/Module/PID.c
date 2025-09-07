#include "PID.h"

#include "DJIMotor.h"
#include "main.h"
#include "math.h"

static PID M3508_pid[4] = {
  {20.0f, 1.5f, 0.0f, 0, 0, 0, 2000, 15000, 0, 0, 0, 0},
  {20.0f, 1.5f, 0.0f, 0, 0, 0, 2000, 15000, 0, 0, 0, 0},
  {20.0f, 1.5f, 0.0f, 0, 0, 0, 2000, 15000, 0, 0, 0, 0},
  {20.0f, 1.5f, 0.0f, 0, 0, 0, 2000, 15000, 0, 0, 0, 0},
};
static PID GM6020_pid_speed = {14.0f, 1.0f, 0.0f, 0, 0, 0, 1500, 12000, 0, 0, 0, 0};
static PID GM6020_pid_position = {4.0f, 0.0f, 1.0f, 0, 0, 0, 1000, 12000, 0, 0, 0, 0};

static PID pid_2006_speed = {20.0f,1.0f, 0.0f, 0,0,0,1000, 8500, 0, 0, 0, 0};
static PID pid_2006_position = {5.0f, 0.0f, 0.0f,0,0,0,0, 450, 0, 0, 0, 0};

PID wz_pid = {-0.066f, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0}; //最大为2

void PID_Init()
{
  for (uint8_t i = 0; i < 4; i++) {
    M3508_motor[i].pid_speed = &M3508_pid[i];
    M3508_motor[i].pid_position = NULL;
  }
  GM6020_motor.pid_speed = &GM6020_pid_speed;
  GM6020_motor.pid_position = &GM6020_pid_position;
  M2006_motor.pid_speed = &pid_2006_speed;
  M2006_motor.pid_position = &pid_2006_position;
}

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
