#ifndef __BSP_USART_H
#define __BSP_USART_H

#include "stdint.h"

#pragma pack(push, 1)
#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  int16_t ch0;
  int16_t ch1;
  int16_t ch2;
  int16_t ch3;
  int16_t ch4;
  unsigned char s1;
  unsigned char s2;

} RC_Ctl_t;

extern uint8_t rx_buffer[18];
extern uint8_t rx_usart6[20];
extern uint8_t rawData[137];
extern RC_Ctl_t rc_ctl;

void USART1_RemoteCallback();
void USART7_RemoteCallback();

#ifdef __cplusplus
}
#endif
#pragma pack(pop)

#endif
