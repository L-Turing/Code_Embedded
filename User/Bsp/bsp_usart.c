#include "bsp_usart.h"

#include "Referee.h"
#include "bsp_crc.h"
#include "main.h"
#include "main_task.h"
#include "math.h"
#include "stdlib.h"
#include "string.h"
#include "usart.h"
#include "usbd_cdc_if.h"
uint8_t rx_buffer[18];
uint8_t rc_buffer[21];
RC_Ctl_t RC_Ctl;
static uint8_t Buf_temp[12] = { 0 };
sendpackge_typedef sendpakge;
recepackge_typedef recepakge;

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef* huart, uint16_t Size)
{
  (void)Size;
  if (huart == &huart3) {  //DT7 DR16
    HAL_UARTEx_ReceiveToIdle_DMA(&huart3, rx_buffer, 18);
    __HAL_DMA_DISABLE_IT(huart3.hdmarx, DMA_IT_HT);
  }
  if (huart == &huart6) {  //图传
    HAL_UARTEx_ReceiveToIdle_DMA(&huart6, rc_buffer, 21);
    __HAL_DMA_DISABLE_IT(huart6.hdmarx, DMA_IT_HT);
  }
}

void Remote_CallbackHandle()
{
  RC_Ctl.ch0 = (rx_buffer[0] | (rx_buffer[1] << 8)) & 0x07ff;
  RC_Ctl.ch0 -= 1024;
  RC_Ctl.ch1 = ((rx_buffer[1] >> 3) | (rx_buffer[2] << 5)) & 0x07ff;
  RC_Ctl.ch1 -= 1024;
  RC_Ctl.ch2 = ((rx_buffer[2] >> 6) | (rx_buffer[3] << 2) | (rx_buffer[4] << 10)) & 0x07ff;
  RC_Ctl.ch2 -= 1024;
  RC_Ctl.ch3 = ((rx_buffer[4] >> 1) | (rx_buffer[5] << 7)) & 0x07ff;
  RC_Ctl.ch3 -= 1024;
  RC_Ctl.ch4 = (rx_buffer[16] | (rx_buffer[17] << 8)) & 0x07ff;
  RC_Ctl.ch4 -= 1024;
  RC_Ctl.s1 = ((rx_buffer[5] >> 4) & 0x000C) >> 2;
  RC_Ctl.s2 = ((rx_buffer[5] >> 4) & 0x0003);

  if (
    (abs(RC_Ctl.ch0) > 660) || (abs(RC_Ctl.ch1) > 660) || (abs(RC_Ctl.ch2) > 660) ||
    (abs(RC_Ctl.ch3) > 660) || (abs(RC_Ctl.ch4) > 660)) {
    memset(&RC_Ctl, 0, sizeof(RC_Ctl_t));
  }
}

void CDC_Receive_Handle(uint8_t* Buf, uint32_t* Len)
{
  for (uint32_t lenth = 0; lenth < *Len; lenth++) {
    Buf_temp[lenth] = Buf[lenth];
  }
}

void Receive_Vision()
{
  if (Verify_CRC16_Check_Sum(Buf_temp, sizeof(Buf_temp)) && (Buf_temp[0] == 0XA5)) {
    recepakge.boolpackage.state = (Buf_temp[1] & 0x03);           //2 0000 0011
    recepakge.boolpackage.id = (Buf_temp[1] & 0x1C) >> 2;         //3 0001 1100
    recepakge.boolpackage.can_shoot = (Buf_temp[1] & 0x20) >> 5;  //1 0010 0000
    recepakge.boolpackage.reserved = (Buf_temp[1] & 0xC0) >> 6;   //2 1100 0000
    memcpy(&recepakge.pitch, Buf_temp + 2, sizeof(float));
    memcpy(&recepakge.yaw, &Buf_temp[6], sizeof(recepakge.yaw));
  }
}

void Send_Vision(
  uint8_t detect_color, uint8_t task_mode, uint8_t reset_tracker, uint8_t is_play, uint8_t reserved,
  float roll, float pitch, float yaw, uint16_t game_time, float timestamp, float bullet_speed)
{
  sendpakge.header = 0x5A;
  sendpakge.boolpackge.detect_color = detect_color;
  sendpakge.boolpackge.task_mode = task_mode;
  sendpakge.boolpackge.reset_tracker = reset_tracker;
  sendpakge.boolpackge.is_play = is_play;
  sendpakge.boolpackge.reserved = reserved;

  sendpakge.roll = roll;
  sendpakge.pitch = pitch;
  sendpakge.yaw = yaw;

  sendpakge.game_time = game_time;
  sendpakge.timestamp = timestamp;
  sendpakge.bullet_speed = bullet_speed;

  memcpy(&sendpakge.datatx_all_u8[0], &sendpakge.header, 1);
  memcpy(&sendpakge.datatx_all_u8[1], &sendpakge.boolpackge, 1);
  memcpy(&sendpakge.datatx_all_u8[2], &sendpakge.roll, 4);
  memcpy(&sendpakge.datatx_all_u8[6], &sendpakge.pitch, 4);
  memcpy(&sendpakge.datatx_all_u8[10], &sendpakge.yaw, 4);
  memcpy(&sendpakge.datatx_all_u8[14], &sendpakge.game_time, 2);
  memcpy(&sendpakge.datatx_all_u8[16], &sendpakge.timestamp, 4);
  memcpy(&sendpakge.datatx_all_u8[20], &sendpakge.bullet_speed, 4);
  Append_CRC16_Check_Sum(sendpakge.datatx_all_u8, sizeof(sendpakge.datatx_all_u8));
  CDC_Transmit_FS(sendpakge.datatx_all_u8, sizeof(sendpakge.datatx_all_u8));
}
