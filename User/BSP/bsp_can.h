#ifndef _BSP_CAN_H
#define _BSP_CAN_H
#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"

typedef enum
{
  DJI = 0,
  DM = 1,
} Motor_Type_param;
extern uint8_t data_can_receive[16][8];
extern uint8_t data_can_send[16][8];

void Can1_Init(void);
void Can2_Init(void);
void CanSend(
  uint8_t which_can, Motor_Type_param type, uint32_t control_id, int16_t motor1, int16_t motor2,
  int16_t motor3, int16_t motor4);
void CanSendBoard(uint8_t which_can, uint32_t control_id, uint8_t * data);

#ifdef __cplusplus
}
#endif
#endif
