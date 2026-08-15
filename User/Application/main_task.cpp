#include "main_task.h"

#include "bsp_usart.h"
#include "cmsis_os.h"
#include "ins_task.h"
#include "main.h"
#include "math.h"
#include "task.h"
#include "usbd_cdc_if.h"

// 函数指针数组----------------------------
typedef uint8_t (*status_t)(uint8_t, uint8_t);
uint8_t LED_Set(uint8_t color, uint8_t state);
//...
status_t op[4] = {LED_Set};

// 全局状态--------------------------------

// FreeRTOS 任务函数-----------------------
void StartINS(void const * argument)
{
  (void)argument;
  INS_Init();
  op[0](0, 1);
  // --- CDC_Transmit_FS 测试 + LED 反馈 ---
  {
    uint8_t cdc_buf[64];
    snprintf((char *)cdc_buf, sizeof(cdc_buf),
             "CDC Test OK! Tick: %lu\r\n", (unsigned long)HAL_GetTick());
    if (CDC_Transmit_FS(cdc_buf, strlen((char *)cdc_buf)) == 0) {  // 0 = USBD_OK
      op[0](0, 0);  // 红灯灭
      op[0](1, 1);  // 绿灯亮 = 发送成功
    } else {
      // 红灯保持亮，绿灯灭 = 发送失败
      op[0](1, 0);
    }
  }
  for (;;) {
    INS_Task();
    osDelay(2);
  }
}

// 其他函数--------------------------------
uint8_t LED_Set(uint8_t color, uint8_t state)
{
  switch (color) {
    case 0:
      HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PinState(state));
      break;
    case 1:
      HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PinState(state));
      break;
    case 2:
      HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, GPIO_PinState(state));
      break;
    default:
      return 1;
  }
  return 0;
}
