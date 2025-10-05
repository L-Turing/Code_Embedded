#ifndef __BSP_CAN_H
#define __BSP_CAN_H

#include "stdint.h"

#pragma pack(push, 1)
#ifdef __cplusplus
extern "C" {
#endif

void Can1_Filter(void);
void Can2_Filter(void);
void CanSend_DJIMotor(
  uint8_t which_can, uint32_t control_id, int16_t motor1, int16_t motor2, int16_t motor3,
  int16_t motor4);
void CanSend_Message(
  uint8_t which_can, uint32_t control_id, uint16_t data1, uint16_t data2, float data3);

#ifdef __cplusplus
}
#endif
#pragma pack(pop)

#endif
