#ifndef __SDCARD_H
#define __SDCARD_H

#include <stdint.h>

#pragma pack(push, 1)

typedef struct {
  uint32_t seq;
  uint32_t timestamp_us;
  uint16_t type;
  uint16_t length;
  uint32_t crc;
  uint8_t payload[248];
} SD_LogBlock;

#pragma pack(pop)

#ifdef __cplusplus
extern "C" {
#endif
void SD_Mount();
void SD_Unmount();

/*
 * SD_Read/SD_Write - file-level operations
 * Parameters:
 *   filename: if NULL, default fileName is used
 *   buf: pointer to data buffer (read or write)
 *   len: number of bytes to read/write (for read, max buffer size)
 *   outLen: (optional) pointer to receive actual bytes read
 * Returns 0 on success, negative on error
 */
int SD_Write(const char * filename, const uint8_t * buf, uint32_t len);
int SD_Read(const char * filename, uint8_t * buf, uint32_t len, uint32_t * outLen);

int SD_LogWriteBlock(const SD_LogBlock *block);
int SD_LogReadLastValidBlock(SD_LogBlock *block, uint32_t *out_seq);
int SD_LogRecover(void);

#ifdef __cplusplus
}
#endif

#endif

//   /* SD 使用示例：挂载 -> 写文件 -> 读回 -> 卸载 */
//   {
//     SD_Mount();
//     const char * demo_file = "example.txt";
//     const char * demo_text = "Hello SD Test from main_task!\r\n";

//     int wret = SD_Write(demo_file, (const uint8_t *)demo_text, (uint32_t)strlen(demo_text));
//     if (wret == 0) {
//       usb_printf("SD demo: write OK (%s)\r\n", demo_file);
//     }
//     else {
//       usb_printf("SD demo: write failed (%d)\r\n", wret);
//     }

//     uint8_t read_buf[128] = {0};
//     uint32_t got = 0;
//     int rret = SD_Read(demo_file, read_buf, sizeof(read_buf) - 1, &got);
//     if (rret == 0) {
//       read_buf[got] = '\0';
//       usb_printf("SD demo: read %lu bytes: %s\r\n", (unsigned long)got, (char *)read_buf);
//     }
//     else {
//       usb_printf("SD demo: read failed (%d)\r\n", rret);
//     }

//     SD_Unmount();
//   }
