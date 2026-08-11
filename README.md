# Code_Embedded

基于 STM32 与 FreeRTOS 的嵌入式实验工程集合，包含两个独立方向：机器人运行数据记录与 BMI088 惯导解算。两个方向分别维护在不同 Git 分支中，请先切换到所需分支再构建。

| 分支 | MCU | 方向 | 当前状态 |
| --- | --- | --- | --- |
| [`Omni_A`](../../tree/Omni_A) | STM32F427 | 机器人运行数据黑匣子、SD 卡日志 | 已具备 FatFS、CRC32 日志块、USB CDC 和基础 FreeRTOS 任务框架 |
| [`Omni_C`](../../tree/Omni_C) | STM32F407 | BMI088 惯导与姿态解算 | 已具备 BMI088 采集、四元数 EKF、温控、CAN/串口和 USB CDC 输出 |

## 快速开始

### 1. 获取并切换分支

```bash
git clone https://github.com/L-Turing/Code_Embedded.git
cd Code_Embedded

# 数据黑匣子分支
git switch Omni_A

# 或：BMI088 惯导分支
git switch Omni_C
```

### 2. 安装依赖

- CMake 3.22 或更新版本
- Ninja
- `arm-none-eabi-gcc` / `arm-none-eabi-g++`
- OpenOCD（可选，用于 SWD 下载）

### 3. 构建

两条分支均提供 `Debug` 和 `Release` 预设：

```bash
cmake --preset Debug
cmake --build --preset Debug
```

生成的 ELF 文件位于 `build/Debug/`：

- `Omni_A`：`Project_1.elf`
- `Omni_C`：`C_Project.elf`

### 4. 烧录

仓库根目录的 [`openocd.cfg`](openocd.cfg) 默认使用 CMSIS-DAP 与 STM32F4 目标配置。连接 SWD 后，例如在 `Omni_C` 分支执行：

```bash
openocd -f openocd.cfg \
  -c "program build/Debug/C_Project.elf verify reset exit"
```

`Omni_A` 请将 ELF 文件名改为 `Project_1.elf`。如使用 ST-Link，请在 `openocd.cfg` 中切换对应接口配置。

## 分支说明

### `Omni_A`：机器人运行数据黑匣子

目标平台为 RoboMaster A 型控制板上的 STM32F427。工程重点是为机器人运行数据提供可恢复的 SD 卡存储基础。

#### 已实现模块

- SDIO + FatFS 文件系统挂载、格式化、普通文件读写
- 固定长度二进制日志块 `SD_LogBlock`
- CRC32 写入校验与“最后有效块”恢复
- USB CDC 初始化和基础发送测试
- FreeRTOS 任务框架：采集、故障监测、存储
- Python 日志解析工具：[`Other/parse_log.py`](Other/parse_log.py)

日志块采用 1 字节对齐，总长 264 字节：

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

示例日志写入和恢复接口位于 [`Module/SDcard.h`](Module/SDcard.h)，存储任务示例位于 [`Application/main_task.cpp`](Application/main_task.cpp)。解析已有日志：

```bash
python Other/parse_log.py log.bin
python Other/parse_log.py log.bin --last
python Other/parse_log.py log.bin -o parsed.json
```

#### 当前范围

现阶段写入任务仍是功能验证代码：周期性写入 `TestData`，尚未接入真实 IMU/CAN 数据，也尚未实现文档规划中的环形缓冲、批量写入、故障前后数据固化和 USB 日志导出协议。

更详细的设计目标见 [`ReadMe/机器人运行数据黑匣子与故障回溯系统.md`](ReadMe/机器人运行数据黑匣子与故障回溯系统.md) 与 [`ReadMe/SD卡存储模块使用说明.md`](ReadMe/SD卡存储模块使用说明.md)。

### `Omni_C`：BMI088 惯导与姿态解算

目标平台为 STM32F407。系统在 FreeRTOS 实时任务中读取 BMI088，通过四元数扩展卡尔曼滤波器估计姿态，并输出原始 IMU 数据。

#### 运行流程

```text
BMI088（SPI）
  -> 陀螺仪零偏/重力标定
  -> 2 ms INS 任务
  -> 四元数 EKF 与坐标系转换
  -> IMU 温度 PID 控制
  -> TIM4 每 10 ms 触发 USB CDC 数据发送
```

#### 主要代码

- 应用入口和任务：[`Src/main.c`](Src/main.c)、[`Src/freertos.c`](Src/freertos.c)
- INS 任务、重力补偿与温控：[`User/Application/ins_task.cpp`](User/Application/ins_task.cpp)
- BMI088 驱动：[`User/Devices/BMI088/`](User/Devices/BMI088)
- 四元数 EKF：[`User/Algorithm/QuaternionEKF.c`](User/Algorithm/QuaternionEKF.c)
- 通用 Kalman、PID 与控制工具：[`User/Algorithm/`](User/Algorithm)、[`User/Include/`](User/Include)
- CAN 与 USB CDC：[`User/Bsp/bsp_can.cpp`](User/Bsp/bsp_can.cpp)、[`User/Bsp/bsp_usart.cpp`](User/Bsp/bsp_usart.cpp)

USB CDC 的发送缓冲区格式为小端序：

| 偏移 | 类型 | 字段 |
| --- | --- | --- |
| 0 | `uint16_t` | 帧头 `0xAA55` |
| 2 | `uint16_t` | 长度 |
| 4 | `float` | `ax` |
| 8 | `float` | `ay` |
| 12 | `float` | `az` |
| 16 | `float` | `gx` |
| 20 | `float` | `gy` |
| 24 | `float` | `gz` |

该有效载荷为 28 字节，发送逻辑位于 `Send_Vsp()`。上位机使用前请核对当前 `CDC_Transmit_FS` 的传入长度与 28 字节缓冲区一致。

## 目录概览

```text
Core/ 或 Src/           STM32CubeMX 生成的启动、外设与 RTOS 文件
Drivers/                STM32 HAL 与 CMSIS
Middlewares/            FreeRTOS、FatFS、USB Device 等第三方组件
User/                   Omni_C 的应用、算法、BSP 与 BMI088 驱动
Application/ Module/    Omni_A 的应用与数据记录模块
cmake/                  ARM GCC / STARM Clang 工具链及 CubeMX 构建清单
```

## 开发说明

- `.ioc` 文件是 STM32CubeMX 配置源；重新生成代码时，请注意保留 `USER CODE` 区域及 CMake 中的用户源文件。
- 代码同时包含 C 与 C++ 源文件；CMake 已移除 C++ 场景下错误引入的 `libob.a` 隐式依赖。
- 目标板、传感器接线和供电安全由使用者负责，请先在无负载、固定姿态下完成 BMI088 标定与 USB 数据验证。

## License

本仓库暂未声明项目许可证。使用、复制或二次发布前，请先联系仓库维护者确认授权范围。
