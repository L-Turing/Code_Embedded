#include "PID.h"

#include "Motor.h"
#include "main.h"
#include "main_task.h"
#include "math.h"

static PID GM6020_pid_speed = { 24.0f, 1.0f, 0.0f, 0, 0, 0, 1500, 16000, 0, 0, 0, 0 ,0,0 };
static PID GM6020_pid_position = { 32.0f, 0.0f, 40.0f, 0, 0, 0, 0, 3000, 0, 0, 0, 0 ,0,0 };

static PID pid_2006_speed = { 18.0f, 1.0f, 0.0f, 0, 0, 0, 500, 9000, 0, 0, 0, 0,0,0 };
static PID pid_2006_position = { 6.0f, 0.0f, 5.0f, 0, 0, 0, 0, 450, 0, 0, 0, 0 ,0,0 };

static PID pid_left_shoot_speed = { 15.0f, 0.0f, 0.0f, 0, 0, 0, 300, 12000, 0, 0, 0, 0 ,0,0 };
static PID pid_right_shoot_speed = { 15.0f, 0.0f, 0.0f, 0, 0, 0, 300, 12000, 0, 0, 0, 0 ,0,0 };

static PID pid_D_yaw_speed = { 2.55f, 0.05f, 0.0f, 0, 0, 0, 2.5f, 7.5f, 0, 0, 0, 0,0,0 };
static PID pid_D_yaw_position = { 0.25f, 0.02f, 0.0f, 0, 0, 0, 2.0f, 18.0f, 0, 0, 0, 0 ,0,0 };

void PID_Init_Motor()
{
  motor_pitch.pid_speed = &GM6020_pid_speed;
  motor_pitch.pid_position = &GM6020_pid_position;

  motor_yaw.pid_speed = &pid_D_yaw_speed;
  motor_yaw.pid_position = &pid_D_yaw_position;

  motor_ammunition.pid_speed = &pid_2006_speed;
  motor_ammunition.pid_position = &pid_2006_position;

  motor_friwheel_left.pid_speed = &pid_left_shoot_speed;
  motor_friwheel_left.pid_position = nullptr;
  motor_friwheel_right.pid_speed = &pid_right_shoot_speed;
  motor_friwheel_right.pid_position = nullptr;
}

static void PID_Error(PID* pid, float feb, float tar);
float PID_Calc(PID* pid, float actual_val, float target_val)
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


static int32_t LOCKED_THRESHOLD = 600;   // 堵转判定次数
static int32_t RECOVER_THRESHOLD = 3600;  // 恢复判定次数
static void PID_Error(PID* pid, float feb, float tar) {
  if (fabsf(tar) < 0.0001f || pid->output < pid->maxO * 0.2f) {
    return; // 跳过检测，保留状态
  }

  if ((fabsf(tar - feb) / fabsf(tar)) > 0.85f) {
    if (pid->Locked_Count < 10000)  // 防止溢出
      pid->Locked_Count++;
  }
  else {
    if (pid->Locked_Count > 0)
      pid->Locked_Count--;
  }

  if (pid->Locked_Count > LOCKED_THRESHOLD) {
    pid->Locked_Judge = 1;
  }
  else if (pid->Locked_Count < RECOVER_THRESHOLD) {
    pid->Locked_Judge = 0;
  }
}