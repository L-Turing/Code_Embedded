#include "bsp_usart.h"

#include "Peripheral.h"
#include "Referee.h"
#include "main.h"
#include "main_task.h"
#include "stdlib.h"
#include "string.h"
#include "usart.h"

uint8_t rx_buffer[18];
uint8_t rawData[137];
uint8_t rx_usart6[20];
RC_Ctl_t rc_ctl;

/**
 * @brief Initializes USART1 and USART7 peripherals for remote control and referee system.
 * This function sets up the USART1 for remote control data reception and USART7 for referee system data reception.
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef * huart, uint16_t Size)
{
  (void)Size;
  if (huart == &huart1) {  //DT7 DR16
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_buffer, 18);
    __HAL_DMA_DISABLE_IT(huart1.hdmarx, DMA_IT_HT);
  }
  if (huart == &huart6) {  //串口收发 
    HAL_UARTEx_ReceiveToIdle_DMA(&huart6, rx_usart6, 20);
    __HAL_DMA_DISABLE_IT(huart6.hdmarx, DMA_IT_HT);
  }
  if (huart == &huart7) {  //裁判系统
    HAL_UARTEx_ReceiveToIdle_DMA(&huart7, rawData, 137);
    __HAL_DMA_DISABLE_IT(huart7.hdmarx, DMA_IT_HT);
  }
}

/**
 * @brief Callback function to process remote control data received via USART1.
 * This function extracts channel values and switch states from the received data.
 * If any channel value exceeds a threshold, it resets the RC_Ctl structure to zero.
 */
void USART1_RemoteCallback()
{
  rc_ctl.ch0 = (rx_buffer[0] | (rx_buffer[1] << 8)) & 0x07ff;
  rc_ctl.ch0 -= 1024;
  rc_ctl.ch1 = ((rx_buffer[1] >> 3) | (rx_buffer[2] << 5)) & 0x07ff;
  rc_ctl.ch1 -= 1024;
  rc_ctl.ch2 = ((rx_buffer[2] >> 6) | (rx_buffer[3] << 2) | (rx_buffer[4] << 10)) & 0x07ff;
  rc_ctl.ch2 -= 1024;
  rc_ctl.ch3 = ((rx_buffer[4] >> 1) | (rx_buffer[5] << 7)) & 0x07ff;
  rc_ctl.ch3 -= 1024;
  rc_ctl.ch4 = (rx_buffer[16] | (rx_buffer[17] << 8)) & 0x07ff;
  rc_ctl.ch4 -= 1024;
  rc_ctl.s1 = ((rx_buffer[5] >> 4) & 0x000C) >> 2;
  rc_ctl.s2 = ((rx_buffer[5] >> 4) & 0x0003);

  if (
    (abs(rc_ctl.ch0) > 661) || (abs(rc_ctl.ch1) > 661) || (abs(rc_ctl.ch2) > 661) ||
    (abs(rc_ctl.ch3) > 661) || (abs(rc_ctl.ch4) > 661)) {
    memset(&rc_ctl, 0, sizeof(RC_Ctl_t));
  }
}

/**
 * @brief Callback function to process referee system data received via USART7.
 * This function decodes the received data and updates the corresponding structures.
 * It handles multiple frames if necessary.
 */
void USART7_RemoteCallback() { decode(rawData); }
