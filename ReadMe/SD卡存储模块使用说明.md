# SD 卡存储模块使用说明

## 1. 模块定位

当前工程中的 SD 卡存储部分用于把机器人运行中的数据落盘，主要面向两类场景：

- 普通文件读写测试
- 带 CRC 校验的日志块存储，用于后续故障回溯与数据分析

当前实现基于 FatFS + SDIO，并配合 CRC32 校验机制，提供了基础的挂载、写入、读取和日志块恢复能力。

---

## 2. 相关文件

- [SDcard.c](Module/SDcard.c)：SD 卡核心实现
- [SDcard.h](Module/SDcard.h)：SD 卡对外接口定义
- [crc32.c](Module/crc32.c)：CRC32 校验实现
- [crc32.h](Module/crc32.h)：CRC32 接口定义
- [FATFS/App/fatfs.c](FATFS/App/fatfs.c)：FatFS 文件系统相关实现

---

## 3. 当前支持的功能

### 3.1 基础文件操作

可以直接对 SD 卡上的文件进行读写：

- `SD_Mount()`：挂载 SD 卡文件系统
- `SD_Unmount()`：卸载 SD 卡文件系统
- `SD_Write()`：向指定文件写入原始字节数据
- `SD_Read()`：从指定文件读取数据

适合做简单的文件测试和调试。

### 3.2 日志块存储

为了适配“黑匣子”与“故障回溯”场景，工程还提供了日志块模式：

- `SD_LogWriteBlock()`：写入一个带 CRC 的日志块
- `SD_LogReadLastValidBlock()`：读取最后一个有效日志块
- `SD_LogRecover()`：尝试恢复最后一个有效日志块

### 3.3 CRC 校验

每个日志块在写入前会计算 CRC32，并在读取时重新校验，保证数据完整性。

---

## 4. 日志块结构

当前日志块定义如下：

```c
typedef struct {
  uint32_t seq;
  uint32_t timestamp_us;
  uint16_t type;
  uint16_t length;
  uint32_t crc;
  uint8_t payload[248];
} SD_LogBlock;
```

字段说明：

- `seq`：序号，用于标记日志块顺序
- `timestamp_us`：时间戳，建议使用统一的硬件计时器值
- `type`：数据类型或事件类型
- `length`：有效 payload 长度
- `crc`：CRC32 校验值
- `payload`：实际存储的数据内容

---

## 5. 使用方法

### 5.1 初始化与挂载

在使用 SD 卡前，先进行挂载：

```c
#include "SDcard.h"

void App_InitSD(void)
{
  SD_Mount();
}
```

### 5.2 写入普通文件

```c
#include "SDcard.h"

const char *demo = "hello sd\r\n";
SD_Write("demo.txt", (const uint8_t *)demo, (uint32_t)strlen(demo));
```

### 5.3 写入日志块

```c
#include "SDcard.h"

SD_LogBlock block;
memset(&block, 0, sizeof(block));
block.seq = 1;
block.timestamp_us = 123456;
block.type = 0x01;
block.length = 8;
memcpy(block.payload, "abc12345", 8);

SD_LogWriteBlock(&block);
```

### 5.4 恢复最后一个有效日志

```c
#include "SDcard.h"

SD_LogRecover();
```

如果系统重启后想恢复最后一段有效日志，可以在启动流程中调用该接口。

### 5.5 读取日志块

```c
#include "SDcard.h"

SD_LogBlock block;
uint32_t seq = 0;
if (SD_LogReadLastValidBlock(&block, &seq) == 0) {
  // 读取成功
}
```

### 5.6 卸载

在结束使用或准备掉电前，建议调用：

```c
SD_Unmount();
```

---

## 6. 使用注意事项

1. 当前版本以“基础可用”为主，适合先完成功能验证和数据落盘测试。
2. 目前日志写入使用单个文件 `log.bin`，属于简单版本，后续可以扩展为追加写入、环形缓冲、文件分段存储。
3. 建议在上层任务中统一调用 SD 卡接口，避免在多个中断上下文中直接频繁操作文件系统。
4. 对于高频采样场景，后续建议再加入缓存队列和独立日志任务，减少 SD 卡写入抖动对实时任务的影响。
5. 如果 SD 卡拔出或写入失败，建议在上层记录对应故障事件。

---

## 7. 推荐调用顺序

建议按下面顺序使用：

```c
SD_Mount();
SD_LogRecover();

// 采集/记录数据
SD_LogWriteBlock(&block);

SD_Unmount();
```

---

## 8. 后续扩展方向

当前实现已经具备基础存储与 CRC 校验能力，后续可以继续扩展为：

- 环形日志文件
- 事件级别的故障记录
- 批量写入与缓存队列
- USB 导出日志
- Python 解析工具接入

如果后续要进一步完善“黑匣子”与“故障回溯”能力，建议重点扩展这几部分。
