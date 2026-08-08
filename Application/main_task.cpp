#include "main_task.h"

#include "Peripheral.h"
#include "SDcard.h"
#include "bsp_dwt.h"
#include "cmsis_os.h"
#include "main.h"
#include "string.h"
#include "usbd_cdc_if.h"

//全局变量
SD_LogBlock block;

void StartImuAcquire(void * argument)
{
  (void)argument;
  SignalMark(type_signal::LED_G_ON);
  for (;;) {
    SignalMark(type_signal::LED_R_Breathe_ON);
    SignalMark(type_signal::LED_G_ON);
    SignalMark(type_signal::LED_1_ON);
    SignalMark(type_signal::LED_2_ON);
    SignalMark(type_signal::LED_3_ON);
    SignalMark(type_signal::LED_4_ON);
    SignalMark(type_signal::LED_5_ON);
    SignalMark(type_signal::LED_6_ON);
    SignalMark(type_signal::LED_7_ON);
    SignalMark(type_signal::LED_8_ON);
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
  SD_Mount();

  // --- CDC_Transmit_FS 测试 ---
  uint8_t cdc_test_buf[64];
  snprintf(
    (char *)cdc_test_buf, sizeof(cdc_test_buf), "CDC Test OK! Tick: %lu\r\n",
    (unsigned long)DWT_GetTimeline_ms());
  uint8_t cdc_result = CDC_Transmit_FS(cdc_test_buf, strlen((char *)cdc_test_buf));
  // cdc_result: USBD_OK (0) = 成功, USBD_BUSY = 忙, USBD_FAIL = 失败

  for (;;) {
    memset(&block, 0, sizeof(block));
    block.seq = 1;
    block.timestamp_us = DWT_GetTimeline_ms();
    block.type = 0x1;                      // 示例类型
    block.length = 8;                      // 示例长度
    memcpy(block.payload, "TestData", 8);  // 示例数据
    SD_LogWriteBlock(&block);

    SD_Unmount();
    osDelay(3000);
  }
}
