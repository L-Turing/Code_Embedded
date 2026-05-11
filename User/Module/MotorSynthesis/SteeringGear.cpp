#include "SteeringGear.h"

#include "main.h"
#include "main_task.h"

//累计角度
void Servo::Servo_Control()
{
  if (this->angle_accu_set > this->angle_max) this->angle_accu_set = this->angle_max;
  if (this->angle_accu_set < this->angle_min) this->angle_accu_set = this->angle_min;

  this->pulse_set = (uint16_t)msp(
    this->angle_accu_set, this->angle_min, this->angle_max, this->pulse_valid_min,
    this->pulse_valid_max);

  switch (this->Channel) {
    case TIM_CHANNEL_1:
      this->tim_pwmHandle->Instance->CCR1 = this->pulse_set;
      break;
    case TIM_CHANNEL_2:
      this->tim_pwmHandle->Instance->CCR2 = this->pulse_set;
      break;
    case TIM_CHANNEL_3:
      this->tim_pwmHandle->Instance->CCR3 = this->pulse_set;
      break;
    case TIM_CHANNEL_4:
      this->tim_pwmHandle->Instance->CCR4 = this->pulse_set;
      break;
  }
}
