#include "crc32.h"

static uint32_t crc32_table[256];
static int crc32_table_ready = 0;

static void CRC32_InitTable(void)
{
  if (crc32_table_ready) {
    return;
  }

  for (uint32_t i = 0; i < 256U; ++i) {
    uint32_t c = i;
    for (int j = 0; j < 8; ++j) {
      if ((c & 1U) != 0U) {
        c = (c >> 1) ^ 0xEDB88320U;
      } else {
        c >>= 1;
      }
    }
    crc32_table[i] = c;
  }
  crc32_table_ready = 1;
}

uint32_t CRC32_Update(uint32_t crc, const uint8_t *data, uint32_t len)
{
  CRC32_InitTable();
  if (data == 0 || len == 0U) {
    return crc;
  }

  uint32_t result = crc ^ 0xFFFFFFFFU;
  for (uint32_t i = 0; i < len; ++i) {
    result = (result >> 8) ^ crc32_table[(result ^ data[i]) & 0xFFU];
  }
  return result ^ 0xFFFFFFFFU;
}

uint32_t CRC32_Calc(const uint8_t *data, uint32_t len)
{
  return CRC32_Update(0xFFFFFFFFU, data, len);
}
