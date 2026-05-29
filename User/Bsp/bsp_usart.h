#ifndef _BSP_USART_H
#define _BSP_USART_H

#include "main.h"
#pragma pack(push, 1)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  uint16_t header;
  uint16_t length;
  float ax;
  float ay;
  float az;
  float gx;
  float gy;
  float gz;
  uint8_t send_buffer[28];
} send_packet_t;

extern send_packet_t send_packet;
void CDC_Receive_Handle(const uint8_t * Buf, const uint32_t * Len);
void Send_Vsp();

#ifdef __cplusplus
}
#endif

#pragma pack(pop)

#endif
