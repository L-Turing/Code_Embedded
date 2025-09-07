#ifndef DAMIAO_H
#define DAMIAO_H

#pragma pack(push, 1)

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

typedef struct
{
  uint16_t state;
  int p_int;
  int v_int;
  int t_int;
  float pos;
  float vel;
  float tor;
  float Tmos;
  float Tcoil;
  float pos_last;
  int32_t cirnum;
  float accumulate_angle;
} DaMiao_Motor;

extern DaMiao_Motor D_yaw;

void Enable_Damiao_Motor(uint8_t which_can, uint32_t id);
void Disable_Damiao_Motor(uint8_t which_can, uint32_t id);
void Drive_Damiao_Motor_MIT(
  uint8_t which_can, uint32_t control_id, float _pos, float _vel, float _KP, float _KD,
  float _torq);
void DaMiao_GetInfo(DaMiao_Motor * D_motor, uint8_t * canbuf_receive);
#ifdef __cplusplus
}
#endif

#pragma pack(pop)

#endif
