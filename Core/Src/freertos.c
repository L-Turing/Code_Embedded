/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for ImuAcquire_TK */
osThreadId_t ImuAcquire_TKHandle;
const osThreadAttr_t ImuAcquire_TK_attributes = {
  .name = "ImuAcquire_TK",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for FaultMonitor_TK */
osThreadId_t FaultMonitor_TKHandle;
const osThreadAttr_t FaultMonitor_TK_attributes = {
  .name = "FaultMonitor_TK",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for Storage_TK */
osThreadId_t Storage_TKHandle;
const osThreadAttr_t Storage_TK_attributes = {
  .name = "Storage_TK",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartImuAcquire(void *argument);
void StartFaultMonitor(void *argument);
void StartStorage(void *argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of ImuAcquire_TK */
  ImuAcquire_TKHandle = osThreadNew(StartImuAcquire, NULL, &ImuAcquire_TK_attributes);

  /* creation of FaultMonitor_TK */
  FaultMonitor_TKHandle = osThreadNew(StartFaultMonitor, NULL, &FaultMonitor_TK_attributes);

  /* creation of Storage_TK */
  Storage_TKHandle = osThreadNew(StartStorage, NULL, &Storage_TK_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartImuAcquire */
/**
* @brief Function implementing the ImuAcquire_TK thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartImuAcquire */
__weak void StartImuAcquire(void *argument)
{
  /* USER CODE BEGIN StartImuAcquire */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartImuAcquire */
}

/* USER CODE BEGIN Header_StartFaultMonitor */
/**
* @brief Function implementing the FaultMonitor_TK thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartFaultMonitor */
__weak void StartFaultMonitor(void *argument)
{
  /* USER CODE BEGIN StartFaultMonitor */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartFaultMonitor */
}

/* USER CODE BEGIN Header_StartStorage */
/**
* @brief Function implementing the Storage_TK thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartStorage */
__weak void StartStorage(void *argument)
{
  /* USER CODE BEGIN StartStorage */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartStorage */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

