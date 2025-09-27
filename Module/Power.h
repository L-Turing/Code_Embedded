#ifndef __POWER_H
#define __POWER_H

#pragma pack(push, 1)

#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"
#include "stdint.h"

typedef struct
{
  uint16_t cap_energy;
  uint16_t input_power;
  uint16_t output_power; 
  uint8_t cap_state;
} CAP;

extern CAP cap;
void Chassis_Power_Control(float * current);

#ifdef __cplusplus
}
#endif

#pragma pack(pop)

#endif
