#include "PID.h"

#include "Motor.h"
#include "main.h"
#include "main_task.h"
#include "math.h"

static PID pid_leftwheel_speed = {12.0f, 0.0f, 0.0f, 0, 0, 0, 0, 9000, 0, 0, 0, 0, 0, 0};
static PID pid_rightwheel_speed = {12.0f, 0.0f, 0.0f, 0, 0, 0, 0, 9000, 0, 0, 0, 0, 0, 0};

void PID_Init_Motor()
{
  motor_leftwheel.pid_speed = &pid_leftwheel_speed;
  motor_leftwheel.pid_position = nullptr;
  motor_rightwheel.pid_speed = &pid_rightwheel_speed;
  motor_rightwheel.pid_position = nullptr;
}

static void PID_Error(PID * pid, float feb, float tar);
float PID_Calc(PID * pid, float actual_val, float target_val)
{
  //堵转判定
  PID_Error(pid, actual_val, target_val);

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

static int32_t LOCKED_THRESHOLD = 600;    // 堵转判定次数
static int32_t RECOVER_THRESHOLD = 3600;  // 恢复判定次数
static void PID_Error(PID * pid, float feb, float tar)
{
  if (fabsf(tar) < 0.0001f || pid->output < pid->maxO * 0.2f) {
    return;  // 跳过检测，保留状态
  }

  if ((fabsf(tar - feb) / fabsf(tar)) > 0.85f) {
    if (pid->Locked_Count < 10000)  // 防止溢出
      pid->Locked_Count++;
  }
  else {
    if (pid->Locked_Count > 0) pid->Locked_Count--;
  }

  if (pid->Locked_Count > LOCKED_THRESHOLD) {
    pid->Locked_Judge = 1;
  }
  else if (pid->Locked_Count < RECOVER_THRESHOLD) {
    pid->Locked_Judge = 0;
  }
}