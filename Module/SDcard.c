#include "SDcard.h"

#include "crc32.h"
#include "fatfs.h"
#include "sdio.h"
#include "stdio.h"
#include "string.h"
#include "usbd_cdc_if.h"

static BYTE work[_MAX_SS];
static char * fileName = "test.txt";
static unsigned int writeLen;
static unsigned int readLen;
static const char * logFileName = "log.bin";
static uint32_t g_last_seq = 0;

static int SD_WriteRaw(const char * filename, const uint8_t * buf, uint32_t len)
{
  if (buf == NULL || len == 0) {
    usb_printf("SD_WriteRaw: invalid buf/len\r\n");
    return -1;
  }

  const char * fname = filename ? filename : fileName;
  FATFS * fs;
  DWORD fre_clust;
  FRESULT fres = f_getfree(SDPath, &fre_clust, &fs);
  if (fres != FR_OK) {
    usb_printf("SD_WriteRaw: f_getfree failed %d\r\n", fres);
    return -2;
  }

  uint64_t free_bytes = (uint64_t)fre_clust * fs->csize * 512U;
  if ((uint64_t)len > free_bytes) {
    usb_printf("SD_WriteRaw: not enough space (%lu bytes required, %llu free)\r\n",
               (unsigned long)len, (unsigned long long)free_bytes);
    return -3;
  }

  retSD = f_open(&SDFile, fname, FA_OPEN_APPEND | FA_WRITE);
  if (retSD == FR_NO_FILE) {
    retSD = f_open(&SDFile, fname, FA_CREATE_NEW | FA_WRITE);
  }
  if (retSD != FR_OK) {
    usb_printf("SD_WriteRaw: f_open failed %d\r\n", retSD);
    return -4;
  }

#if defined(SCB) && defined(__DCACHE_PRESENT)
#if (__DCACHE_PRESENT == 1U)
  SCB_CleanDCache_by_Addr((uint32_t *)buf, (int)len);
#endif
#endif

  retSD = f_write(&SDFile, buf, len, &writeLen);
  if (retSD != FR_OK) {
    usb_printf("SD_WriteRaw: f_write failed %d\r\n", retSD);
    f_close(&SDFile);
    return -5;
  }

  retSD = f_sync(&SDFile);
  if (retSD != FR_OK) {
    usb_printf("SD_WriteRaw: f_sync failed %d\r\n", retSD);
    f_close(&SDFile);
    return -6;
  }

  f_close(&SDFile);
  return 0;
}

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
  return SD_WriteRaw(filename, buf, len);
}

int SD_LogWriteBlock(const SD_LogBlock *block)
{
  if (block == NULL) {
    usb_printf("SD_LogWriteBlock: invalid block\r\n");
    return -1;
  }

  SD_LogBlock local = *block;
  local.crc = CRC32_Calc((const uint8_t *)&local, sizeof(local) - sizeof(local.crc));
  local.crc = local.crc;

  if (SD_WriteRaw(logFileName, (const uint8_t *)&local, sizeof(local)) != 0) {
    return -2;
  }

  g_last_seq = local.seq;
  return 0;
}

int SD_LogReadLastValidBlock(SD_LogBlock *block, uint32_t *out_seq)
{
  if (block == NULL) {
    return -1;
  }

  FIL file;
  if (f_open(&file, logFileName, FA_OPEN_EXISTING | FA_READ) != FR_OK) {
    return -2;
  }

  UINT bytes = 0;
  SD_LogBlock temp;
  uint32_t last_seq = 0;
  int found = 0;

  while (f_read(&file, &temp, sizeof(temp), &bytes) == FR_OK && bytes > 0U) {
    if (bytes != sizeof(temp)) {
      break;
    }

    uint32_t crc_calc = CRC32_Calc((const uint8_t *)&temp, sizeof(temp) - sizeof(temp.crc));
    if (crc_calc == temp.crc) {
      *block = temp;
      last_seq = temp.seq;
      found = 1;
    }
  }

  f_close(&file);
  if (out_seq) {
    *out_seq = last_seq;
  }
  return found ? 0 : -3;
}

int SD_LogRecover(void)
{
  SD_LogBlock block;
  uint32_t seq = 0;
  if (SD_LogReadLastValidBlock(&block, &seq) == 0) {
    g_last_seq = seq;
    usb_printf("SD_LogRecover: recovered seq=%lu\r\n", (unsigned long)seq);
    return 0;
  }
  usb_printf("SD_LogRecover: no valid block found\r\n");
  return -1;
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
