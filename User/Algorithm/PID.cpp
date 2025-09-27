#include "PID.h"

#include "Motor.h"
#include "DaMiao.h"
#include "main.h"
#include "main_task.h"
#include "math.h"

static PID GM6020_pid_speed = { 24.0f, 1.0f, 0.0f, 0, 0, 0, 1500, 16000, 0, 0, 0, 0 };
static PID GM6020_pid_position = { 32.0f, 0.0f, 40.0f, 0, 0, 0, 0, 3000, 0, 0, 0, 0 };

static PID pid_2006_speed = { 20.0f, 1.0f, 0.0f, 0, 0, 0, 1000, 9000, 0, 0, 0, 0 };
static PID pid_2006_position = { 7.0f, 0.0f, 0.0f, 0, 0, 0, 0, 450, 0, 0, 0, 0 };

static PID pid_left_shoot_speed = { 15.0f, 0.0f, 0.0f, 0, 0, 0, 300, 12000, 0, 0, 0, 0 };
static PID pid_right_shoot_speed = { 15.0f, 0.0f, 0.0f, 0, 0, 0, 300, 12000, 0, 0, 0, 0 };

static PID pid_D_yaw_speed = { 2.55f, 0.05f, 0.0f, 0, 0, 0, 2.5f, 7.5f, 0, 0, 0, 0 };
static PID pid_D_yaw_position = { 0.25f, 0.02f, 0.0f, 0, 0, 0, 2.0f, 18.0f, 0, 0, 0, 0 };

void PID_Init_()
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

float PID_Calc(PID* pid, float actual_val, float target_val)
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
