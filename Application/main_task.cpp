#include "main_task.h"

#include "Peripheral.h"
#include "SDcard.h"
#include "bsp_dwt.h"
#include "cmsis_os.h"
#include "main.h"
#include "string.h"

//全局变量
SD_LogBlock block;

void StartImuAcquire(void * argument)
{
  (void)argument;
  SignalMark(type_signal::LED_G_ON);
  for (;;) {
    osDelay(1);
  }
}

void StartFaultMonitor(void * argument)
{
  (void)argument;
  for (;;) {
    osDelay(1);
  }
}

void StartStorage(void * argument)
{
  (void)argument;
  //SD_Mount();
  for (;;) {
    // memset(&block, 0, sizeof(block));
    // block.seq = 1;
    // block.timestamp_us = DWT_GetTimeline_ms();
    // block.type = 0x1;                      // 示例类型
    // block.length = 8;                      // 示例长度
    // memcpy(block.payload, "TestData", 8);  // 示例数据
    // SD_LogWriteBlock(&block);

    //SD_Unmount();
    osDelay(2);
  }
}
