#ifndef _BSP_CAN_H
#define _BSP_CAN_H

#include "main.h"
#pragma pack(push, 1)

#ifdef __cplusplus
extern "C" {
#endif

void Can_Filter_Init();
void Can_Msg_Send(uint8_t which_can, uint32_t send_id, const uint8_t * data);

#ifdef __cplusplus
}
#endif

#pragma pack(pop)

#endif
