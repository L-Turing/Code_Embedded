#include "bsp_usart.h"

#include "INS_Task.h"
#include "main_task.h"
#include "usart.h"
#include "usbd_cdc_if.h"

send_packet_t send_packet;

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef * huart, uint16_t Size)
{
  (void)Size;
  if (huart == &huart3) {
    HAL_UARTEx_ReceiveToIdle_DMA(&huart3, receive_usart_buffer[3], 18);
    __HAL_DMA_DISABLE_IT(huart3.hdmarx, DMA_IT_HT);
  }
  if (huart == &huart6) {
    HAL_UARTEx_ReceiveToIdle_DMA(&huart6, receive_usart_buffer[6], 21);
    __HAL_DMA_DISABLE_IT(huart6.hdmarx, DMA_IT_HT);
  }
}

void CDC_Receive_Handle(const uint8_t * Buf, const uint32_t * Len)
{
  memcpy(receive_usart_buffer[0], Buf, *Len);
}

void Send_Vsp()
{
  send_packet.header = 0xAA55;
  send_packet.length = sizeof(send_packet);
  send_packet.ax = INS.Accel[X];
  send_packet.ay = INS.Accel[Y];
  send_packet.az = INS.Accel[Z];
  send_packet.gx = INS.Gyro[X];
  send_packet.gy = INS.Gyro[Y];
  send_packet.gz = INS.Gyro[Z];

  memcpy(send_packet.send_buffer, &send_packet.header, 2);
  memcpy(send_packet.send_buffer + 2, &send_packet.length, 2);
  memcpy(send_packet.send_buffer + 4, &send_packet.ax, 4);
  memcpy(send_packet.send_buffer + 8, &send_packet.ay, 4);
  memcpy(send_packet.send_buffer + 12, &send_packet.az, 4);
  memcpy(send_packet.send_buffer + 16, &send_packet.gx, 4);
  memcpy(send_packet.send_buffer + 20, &send_packet.gy, 4);
  memcpy(send_packet.send_buffer + 24, &send_packet.gz, 4);

  CDC_Transmit_FS(send_packet.send_buffer, sizeof(send_packet));
}
