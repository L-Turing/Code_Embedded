#include "bsp_usart.h"

#include "Peripheral.h"
#include "main.h"
#include "main_task.h"
#include "stdlib.h"
#include "string.h"
#include "usart.h"

DT7_DR16_t dt7_dr16;
PS2_t PS2;
Flag_t flag;

void (*USART_Callback[9])();

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef * huart, uint16_t Size)
{
  (void)Size;
  if (huart == &huart1) {  //DT7&&DR16
    USART_Callback[1]();
  }
  if (huart == &huart6) {  //PS2手柄
    USART_Callback[6]();
  }
}

void PS2_Handle()
{
  HAL_UARTEx_ReceiveToIdle_DMA(&huart6, PS2.rx_ps2_uint, sizeof(PS2.rx_ps2_uint));
  __HAL_DMA_DISABLE_IT(huart6.hdmarx, DMA_IT_HT);

  memcpy(PS2.frame_header, PS2.rx_ps2_uint, 2);
  memcpy(&PS2.operation_code, PS2.rx_ps2_uint + 2, 1);
  memcpy(&PS2.length, PS2.rx_ps2_uint + 3, 1);
  memcpy(&PS2.identifier, PS2.rx_ps2_uint + 4, 1);

  if (
    PS2.frame_header[0] == 0X57 && PS2.frame_header[1] == 0XAB && PS2.operation_code == 0X88 &&
    PS2.length == 0X1E && PS2.identifier == 0X00) {
    PS2.select = PS2.rx_ps2_uint[6] & 0X01;
    PS2.mode = (PS2.rx_ps2_uint[6] & 0X10) >> 4;
    PS2.start = (PS2.rx_ps2_uint[6] & 0X02) >> 1;
    PS2.left_joystick_z = (PS2.rx_ps2_uint[6] & 0X04) >> 2;
    PS2.right_joystick_z = (PS2.rx_ps2_uint[6] & 0X08) >> 3;

    memcpy(&PS2.left_joystick_x, PS2.rx_ps2_uint + 9, 1);
    memcpy(&PS2.left_joystick_y, PS2.rx_ps2_uint + 8, 1);
    memcpy(&PS2.right_joystick_x, PS2.rx_ps2_uint + 11, 1);
    memcpy(&PS2.right_joystick_y, PS2.rx_ps2_uint + 10, 1);

    memcpy(&PS2.left_x[0], PS2.rx_ps2_uint + 14, 1);
    memcpy(&PS2.left_x[1], PS2.rx_ps2_uint + 15, 1);
    memcpy(&PS2.left_y[0], PS2.rx_ps2_uint + 13, 1);
    memcpy(&PS2.left_y[1], PS2.rx_ps2_uint + 12, 1);

    memcpy(&PS2.right_x[0], PS2.rx_ps2_uint + 16, 1);
    memcpy(&PS2.right_x[1], PS2.rx_ps2_uint + 18, 1);
    memcpy(&PS2.right_y[0], PS2.rx_ps2_uint + 19, 1);
    memcpy(&PS2.right_y[1], PS2.rx_ps2_uint + 17, 1);

    memcpy(&PS2.left[0], PS2.rx_ps2_uint + 20, 1);
    memcpy(&PS2.left[1], PS2.rx_ps2_uint + 22, 1);
    memcpy(&PS2.right[0], PS2.rx_ps2_uint + 21, 1);
    memcpy(&PS2.right[1], PS2.rx_ps2_uint + 23, 1);

    memcpy(&PS2.serial_number, PS2.rx_ps2_uint + 32, 1);
    memcpy(&PS2.verify, PS2.rx_ps2_uint + 33, 1);
  }
}

void DT7_DR16_Handle()
{
  HAL_UARTEx_ReceiveToIdle_DMA(&huart1, dt7_dr16.rx_dt7_dr16, 18);
  __HAL_DMA_DISABLE_IT(huart1.hdmarx, DMA_IT_HT);

  dt7_dr16.ch0 = (dt7_dr16.rx_dt7_dr16[0] | (dt7_dr16.rx_dt7_dr16[1] << 8)) & 0x07ff;
  dt7_dr16.ch0 -= 1024;
  dt7_dr16.ch1 = ((dt7_dr16.rx_dt7_dr16[1] >> 3) | (dt7_dr16.rx_dt7_dr16[2] << 5)) & 0x07ff;
  dt7_dr16.ch1 -= 1024;
  dt7_dr16.ch2 = ((dt7_dr16.rx_dt7_dr16[2] >> 6) | (dt7_dr16.rx_dt7_dr16[3] << 2) |
                  (dt7_dr16.rx_dt7_dr16[4] << 10)) &
                 0x07ff;
  dt7_dr16.ch2 -= 1024;
  dt7_dr16.ch3 = ((dt7_dr16.rx_dt7_dr16[4] >> 1) | (dt7_dr16.rx_dt7_dr16[5] << 7)) & 0x07ff;
  dt7_dr16.ch3 -= 1024;
  dt7_dr16.ch4 = (dt7_dr16.rx_dt7_dr16[16] | (dt7_dr16.rx_dt7_dr16[17] << 8)) & 0x07ff;
  dt7_dr16.ch4 -= 1024;
  dt7_dr16.s1 = ((dt7_dr16.rx_dt7_dr16[5] >> 4) & 0x000C) >> 2;
  dt7_dr16.s2 = ((dt7_dr16.rx_dt7_dr16[5] >> 4) & 0x0003);
}

static uint8_t Buf_temp[20] = {0};
void CDC_Receive_Handle(uint8_t * Buf, uint32_t * Len)
{
  for (uint32_t lenth = 0; lenth < *Len; lenth++) {
    Buf_temp[lenth] = Buf[lenth];
  }
}
