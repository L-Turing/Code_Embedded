#include "Peripheral.h"

#include "cmsis_os.h"
#include "gpio.h"
#include "tim.h"

// SignalMark function to control various signals like LEDs and buzzer
// The function takes a type_signal enum as an argument to determine which signal to control
// It uses a switch statement to handle different cases for turning signals on or off
void SignalMark(type_signal sign)
{
  switch (sign) {
    case LED_G_ON:
      HAL_GPIO_WritePin(LED_green_GPIO_Port, LED_green_Pin, GPIO_PIN_RESET);
      break;
    case LED_1_ON:
      HAL_GPIO_WritePin(GPIOG, GPIO_PIN_1, GPIO_PIN_RESET);
      break;
    case LED_2_ON:
      HAL_GPIO_WritePin(GPIOG, GPIO_PIN_2, GPIO_PIN_RESET);
      break;
    case LED_3_ON:
      HAL_GPIO_WritePin(GPIOG, GPIO_PIN_3, GPIO_PIN_RESET);
      break;
    case LED_4_ON:
      HAL_GPIO_WritePin(GPIOG, GPIO_PIN_4, GPIO_PIN_RESET);
      break;
    case LED_5_ON:
      HAL_GPIO_WritePin(GPIOG, GPIO_PIN_5, GPIO_PIN_RESET);
      break;
    case LED_6_ON:
      HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_RESET);
      break;
    case LED_7_ON:
      HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_RESET);
      break;
    case LED_8_ON:
      HAL_GPIO_WritePin(GPIOG, GPIO_PIN_8, GPIO_PIN_RESET);
      break;
    case Buzzer_ON:
      htim12.Instance->CCR1 = 200;
      break;
    case LED_R_ON:
      htim1.Instance->CCR2 = 0;
      break;
    case LED_R_Breathe_ON:
      for (uint16_t ccr_duty = 0; ccr_duty < htim1.Instance->ARR; ccr_duty++) {
        htim1.Instance->CCR2 = ccr_duty;
        osDelay(1);
      }
      for (uint16_t ccr_duty = htim1.Instance->ARR; ccr_duty > 0; ccr_duty--) {
        htim1.Instance->CCR2 = ccr_duty;
        osDelay(1);
      }
      break;

    case LED_G_OFF:
      HAL_GPIO_WritePin(LED_green_GPIO_Port, LED_green_Pin, GPIO_PIN_SET);
      break;
    case LED_1_OFF:
      HAL_GPIO_WritePin(GPIOG, GPIO_PIN_1, GPIO_PIN_SET);
      break;
    case LED_2_OFF:
      HAL_GPIO_WritePin(GPIOG, GPIO_PIN_2, GPIO_PIN_SET);
      break;
    case LED_3_OFF:
      HAL_GPIO_WritePin(GPIOG, GPIO_PIN_3, GPIO_PIN_SET);
      break;
    case LED_4_OFF:
      HAL_GPIO_WritePin(GPIOG, GPIO_PIN_4, GPIO_PIN_SET);
      break;
    case LED_5_OFF:
      HAL_GPIO_WritePin(GPIOG, GPIO_PIN_5, GPIO_PIN_SET);
      break;
    case LED_6_OFF:
      HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_SET);
      break;
    case LED_7_OFF:
      HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_SET);
      break;
    case LED_8_OFF:
      HAL_GPIO_WritePin(GPIOG, GPIO_PIN_8, GPIO_PIN_SET);
      break;
    case Buzzer_OFF:
      htim12.Instance->CCR1 = 1000;
      break;
    case LED_R_OFF:
      htim1.Instance->CCR2 = 1000;
      break;
    case LED_R_Breathe_OFF:
      htim1.Instance->CCR2 = 1000;
      break;

    default:
      break;
  }
}

/*
LED1-4:M3508[4]&蜂鸣器
LED5:超级电容&蜂鸣器
LED6:裁判系统串口线&蜂鸣器
LED7:板间通信&蜂鸣器
LED8:M2006&蜂鸣器
*/
