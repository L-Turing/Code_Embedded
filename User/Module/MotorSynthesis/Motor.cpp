#include "Motor.h"

#include <cmath>
#include <iostream>

#include "PID.h"
#include "main_task.h"

void Motor_Class::SetWheel(float wheel_tar_rpm)
{
  PID_Calc(this->pid_speed, this->speed_rpm_out, wheel_tar_rpm);
  this->output = this->pid_speed->output;
}

bool Motor_Class::Lost_Judge()
{
  if (this->flag_connect > 100) {
    this->flag_connect = 100;  //已经掉线，防止溢出
    return false;
  }
  else {
    return true;
  }
}

void Motor_Class::Update_Info()
{
  switch (this->motor_type) {
    case motor_types::M3508:
      this->reduction_ratio = 19.0f;
      break;
    case motor_types::M2006:
      // this->reduction_ratio = 36.0f;
      this->reduction_ratio = 19.2f;
      break;
    case motor_types::GM6020:
      this->reduction_ratio = 1.0f;
      break;
    case motor_types::DaMiao4310:
      this->reduction_ratio = 1.0f;
      break;
    case motor_types::DaMiao3507:
      this->reduction_ratio = 1.0f;
      break;

    default:
      this->reduction_ratio = 1.0f;
      break;
  }

  if (
    this->motor_type == motor_types::GM6020 || this->motor_type == motor_types::M2006 ||
    this->motor_type == motor_types::M3508) {
    if (this->ecd - this->last_ecd < -4096) {  // 转子过圈判断
      this->circle_number_rotor++;
    }
    else if (this->ecd - this->last_ecd > 4096) {
      this->circle_number_rotor--;
    }

    this->accumlate_rad_rotor = (float)((this->circle_number_rotor) * 8192 + this->last_ecd) /
                                8192.0f * 2 * PI;  // 转子累计角度 rad
    this->accumlate_rad_out =
      this->accumlate_rad_rotor / this->reduction_ratio;  // 输出轴累计角度 rad
  }
  else if (
    this->motor_type == motor_types::DaMiao4310 || this->motor_type == motor_types::DaMiao3507) {
    if (this->ecd - this->last_ecd < -3.141593f) {  // 转子过圈判断
      this->circle_number_rotor++;
    }
    else if (this->ecd - this->last_ecd > 3.141593f) {
      this->circle_number_rotor--;
    }
    this->accumlate_rad_rotor =
      (float)((this->circle_number_rotor) * 2 * PI + this->last_ecd);  // 转子累计角度 rad
    this->accumlate_rad_out =
      this->accumlate_rad_rotor / this->reduction_ratio;  // 输出轴累计角度 rad
  }

  this->ecd_rad_rotor = (float)(this->ecd) / 8192.0f * 2.0f * PI - PI;  // 转子编码器值 rad  -PI~PI
  this->ecd_rad_out = fmod(this->accumlate_rad_out, 2 * PI);            //输出轴编码器值 rad  -PI~PI

  this->speed_rpm_out = (float)(this->speed_rpm_rotor) / this->reduction_ratio;  // 输出轴转速 rpm
  this->speed_rad_rotor = (float)(this->speed_rpm_rotor) * 2.0f * PI / 60.0f;    // 转子转速 rad/s
  this->speed_rad_out = this->speed_rad_rotor / this->reduction_ratio;           // 输出轴转速 rad/s
}

void DJIMotor_Class::Update_Status(uint8_t * canbuf_receive)
{
  this->last_ecd = this->ecd;
  this->ecd = (int16_t)((canbuf_receive[0] << 8) | canbuf_receive[1]);
  this->speed_rpm_rotor = (int16_t)((canbuf_receive[2] << 8) | canbuf_receive[3]);  // 转子转速 rpm
  this->given_current = (int16_t)((canbuf_receive[4] << 8) | canbuf_receive[5]);
  this->temperature = (int8_t)(canbuf_receive[6]);

  this->flag_connect++;
  this->flag_state = this->Lost_Judge();
  if (this->flag_state == false)
    SignalMark(this->sign_mark);
  else
    SignalMark(static_cast<type_signal>(static_cast<int>(this->sign_mark) + 100));

  this->Update_Info();
}
