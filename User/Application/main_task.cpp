#include "main_task.h"

#include "bsp_usart.h"
#include "cmsis_os.h"
#include "ins_task.h"
#include "main.h"
#include "math.h"
#include "task.h"

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
