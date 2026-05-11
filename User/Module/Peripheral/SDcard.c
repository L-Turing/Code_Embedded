#include "SDcard.h"

#include "fatfs.h"
#include "sdio.h"
#include "stdio.h"
#include "string.h"
#include "usbd_cdc_if.h"

static BYTE work[_MAX_SS];
static char * fileName = "test.txt";
static unsigned int writeLen;
static unsigned int readLen;

// 挂载
void SD_Mount()
{
  retSD = f_mount(&SDFatFS, SDPath, 1);
  switch (retSD) {
    case FR_NO_FILESYSTEM:  // FM_FAT32
    {
      retSD = f_mkfs(SDPath, FM_FAT32, 512, work, sizeof(work));
      if (retSD != FR_OK) {
        usb_printf("SD_Mount: f_mkfs failed %d\r\n", retSD);
        return;
      }
    } break;
    case FR_OK:
      break;
    default:
      usb_printf("SD_Mount: f_mount failed %d\r\n", retSD);
      return;
  }
}

// 卸载
void SD_Unmount()
{
  if (f_mount(NULL, SDPath, 1) != FR_OK) {
    usb_printf("SD_Unmount: f_mount(NULL) failed\r\n");
  }
  return;
}

// 写（写入传入缓冲区）
int SD_Write(const char * filename, const uint8_t * buf, uint32_t len)
{
  if (buf == NULL || len == 0) {
    usb_printf("SD_Write: invalid buf/len\r\n");
    return -1;
  }
  const char * fname = filename ? filename : fileName;

  /* 检查卡是否挂载并且有足够空间（优先使用 FatFs 空间信息） */
  FATFS * fs;
  DWORD fre_clust;
  FRESULT fres = f_getfree(SDPath, &fre_clust, &fs);
  if (fres != FR_OK) {
    usb_printf("SD_Write: f_getfree failed %d\r\n", fres);
    return -2;
  }
  uint64_t free_bytes = (uint64_t)fre_clust * fs->csize * 512U;
  if ((uint64_t)len > free_bytes) {
    usb_printf(
      "SD_Write: not enough space (%lu bytes required, %llu free)\r\n", (unsigned long)len,
      (unsigned long long)free_bytes);
    return -3;
  }

  /* 打开文件 */
  retSD = f_open(&SDFile, fname, FA_CREATE_ALWAYS | FA_WRITE);
  if (retSD != FR_OK) {
    usb_printf("SD_Write: f_open failed %d\r\n", retSD);
    return -4;
  }

  /* D-cache 与 DMA 的协同：若 MCU 有 D-cache 并且 diskio 使用 DMA，需要清缓存 */
#if defined(SCB) && defined(__DCACHE_PRESENT)
#if (__DCACHE_PRESENT == 1U)
  SCB_CleanDCache_by_Addr((uint32_t *)buf, (int)len);
#endif
#endif

  retSD = f_write(&SDFile, buf, len, &writeLen);
  if (retSD != FR_OK) {
    usb_printf("SD_Write: f_write failed %d\r\n", retSD);
    f_close(&SDFile);
    return -5;
  }

  /* 确保写入落盘 */
  retSD = f_sync(&SDFile);
  if (retSD != FR_OK) {
    usb_printf("SD_Write: f_sync failed %d\r\n", retSD);
    f_close(&SDFile);
    return -6;
  }

  f_close(&SDFile);
  return 0;
}

// 读（从文件读取到传入缓冲区）
int SD_Read(const char * filename, uint8_t * buf, uint32_t len, uint32_t * outLen)
{
  if (buf == NULL || len == 0) {
    usb_printf("SD_Read: invalid buf/len\r\n");
    return -1;
  }
  const char * fname = filename ? filename : fileName;

  retSD = f_open(&SDFile, fname, FA_OPEN_EXISTING | FA_READ);
  if (retSD != FR_OK) {
    usb_printf("SD_Read: f_open failed %d\r\n", retSD);
    return -2;
  }

  /* 读取最多 len 字节 */
  retSD = f_read(&SDFile, buf, len, &readLen);
  if (retSD != FR_OK) {
    usb_printf("SD_Read: f_read failed %d\r\n", retSD);
    f_close(&SDFile);
    return -3;
  }

  /* 如果存在 D-cache 并且使用 DMA，则需要失效缓存以保证 CPU 读取到最新数据 */
#if defined(SCB) && defined(__DCACHE_PRESENT)
#if (__DCACHE_PRESENT == 1U)
  SCB_InvalidateDCache_by_Addr((uint32_t *)buf, (int)readLen);
#endif
#endif

  f_close(&SDFile);
  if (outLen) *outLen = readLen;
  return 0;
}
