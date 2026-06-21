#ifndef __STEERINGGEAR_H
#define __STEERINGGEAR_H

#include "Peripheral.h"
#include "tim.h"
#include "stdint.h"

#pragma pack(push, 1)

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

class Servo
{
private:
  uint16_t pulse_set;

public:
  TIM_HandleTypeDef * tim_pwmHandle;
  uint8_t Channel;
  float angle_min;  //deg
  float angle_max;  //deg
  uint32_t pulse_valid_min;
  uint32_t pulse_valid_max;

  float angle_abs_set{};
  float angle_accu_set{};

  explicit Servo(
    TIM_HandleTypeDef * tim_pwmHandle_t, uint8_t Channel_t, float angle_min_t, float angle_max_t,
    uint32_t pulse_valid_min_t, uint32_t pulse_valid_max_t)
  : tim_pwmHandle(tim_pwmHandle_t),
    Channel(Channel_t),
    angle_min(angle_min_t),
    angle_max(angle_max_t),
    pulse_valid_min(pulse_valid_min_t),
    pulse_valid_max(pulse_valid_max_t)
  {
  }

  void Servo_Control();
};

#endif

#pragma pack(pop)

#endif
