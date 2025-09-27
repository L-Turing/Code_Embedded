#ifndef _BSP_USART_H
#define _BSP_USART_H

#pragma pack(push, 1)

#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"

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

typedef struct
{
  uint8_t detect_color : 1;  // 0-red 1-blue
  uint8_t task_mode : 2;
  uint8_t reset_tracker : 1;
  uint8_t is_play : 1;
  uint8_t reserved : 3;
} bool_sendpackge_typedef;

typedef struct
{
  uint8_t header;
  bool_sendpackge_typedef boolpackge;
  float roll;
  float pitch;
  float yaw;
  uint16_t game_time;
  float timestamp;
  float bullet_speed;
  uint16_t checksum;
  uint8_t datatx_all_u8[26];
} sendpackge_typedef; 

typedef struct
{
  uint8_t state : 2;
  uint8_t id : 3;
  uint8_t can_shoot : 1;
  uint8_t reserved : 2;
} bool_receive_typedef;

typedef struct
{
  uint8_t header;
  bool_receive_typedef boolpackage;
  float pitch;
  float yaw;
  uint16_t checksum;
  uint8_t datarx_all_u8[12];
} recepackge_typedef;

extern uint8_t rx_buffer[18];
extern uint8_t rc_buffer[21];
extern RC_Ctl_t RC_Ctl;
extern sendpackge_typedef sendpakge;
extern recepackge_typedef recepakge;

void CDC_Receive_Handle(uint8_t * Buf, uint32_t * Len);
void Remote_CallbackHandle();
void Receive_Vision();
void Send_Vision(
  uint8_t detect_color, uint8_t task_mode, uint8_t reset_tracker, uint8_t is_play, uint8_t reserved,
  float roll, float pitch, float yaw, uint16_t game_time, float timestamp, float bullet_speed);

#ifdef __cplusplus
}
#endif

#pragma pack(pop)

#endif
