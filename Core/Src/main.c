/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include "dma.h"
#include "fatfs.h"
#include "gpio.h"
#include "iwdg.h"
#include "sdio.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "PID.h"
#include "Peripheral.h"
#include "bsp_dwt.h"
#include "bsp_usart.h"
#include "buzzer.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
uint32_t running_time = 0;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  // DWT_Init(168);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_USB_DEVICE_Init();
  MX_TIM12_Init();
  MX_USART1_UART_Init();
  // MX_IWDG_Init();
  MX_USART6_UART_Init();
  MX_TIM5_Init();
  MX_TIM4_Init();
  MX_SDIO_SD_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */

  // PID_Init_Motor();

  // HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_4);  //PI0
  // HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_3);  //PH12
  // HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_2);  //PH11
  // HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_1);  //PH10
  // HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);  //PD15

  // HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);   //呼吸灯
  //HAL_TIM_PWM_Start(&htim12, TIM_CHANNEL_1);  //蜂鸣器
  // __HAL_TIM_CLEAR_IT(&htim2, TIM_IT_UPDATE);  //计时
  // HAL_TIM_Base_Start_IT(&htim2);

  // USART_Callback[1] = DT7_DR16_Handle;
  // USART_Callback[6] = PS2_Handle;
  // HAL_UARTEx_ReceiveToIdle_DMA(&huart1, dt7_dr16.rx_dt7_dr16, sizeof(dt7_dr16.rx_dt7_dr16));
  // __HAL_DMA_DISABLE_IT(huart1.hdmarx, DMA_IT_HT);
  // HAL_UARTEx_ReceiveToIdle_DMA(&huart6, PS2.rx_ps2_uint, sizeof(PS2.rx_ps2_uint));
  // __HAL_DMA_DISABLE_IT(huart6.hdmarx, DMA_IT_HT);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    /* USER CODE END WHILE */
    gala_you();
    HAL_Delay(10000);
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 6;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType =
    RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM3 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef * htim)
{
  /* USER CODE BEGIN Callback 0 */
  static uint16_t TIM2_Cnt = 0;
  if (htim->Instance == TIM2) {  //1ms一次中断
    TIM2_Cnt++;
    if (TIM2_Cnt >= 50) {  //50ms
      // TIM2_Cnt = 0;
      // running_time++;
      // //Send_Vsp();     //发送数据到上位机
      // Receive_Vsp();  //接收来自上位机的数据
    }
  }
  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM3) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1) {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t * file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
