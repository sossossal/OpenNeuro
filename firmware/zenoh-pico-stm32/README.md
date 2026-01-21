# Zenoh-Pico STM32H7 移植

## 概述

本目录包含 Zenoh-Pico 在 STM32H7 系列 MCU 上的移植。STM32H7 提供更高的性能和实时性，适合对延迟敏感的应用。

## 硬件要求

- **STM32H7 开发板**：NUCLEO-H745ZI-Q 或 STM32H743I-EVAL
- **以太网模块**：板载或外接以太网 PHY
- **调试器**：ST-Link v2 或更高版本

## 快速开始

### 1. 安装开发工具

```bash
# 方式 1：STM32CubeIDE（集成开发环境，推荐）
# 下载地址: https://www.st.com/en/development-tools/stm32cubeide.html

# 方式 2：ARM GCC + CMake
apt-get install gcc-arm-linux-gnueabihf cmake
```

### 2. 导入项目

```bash
# 使用 STM32CubeIDE
File → Import Projects from File System...
# 选择当前目录

# 或使用命令行编译
cd firmware/zenoh-pico-stm32
mkdir build && cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../arm-toolchain.cmake ..
make -j4
```

### 3. 配置以太网

编辑 `src/config.h`：

```c
#define ETH_MAC_ADDR        {0x00, 0x00, 0x00, 0x00, 0x00, 0x01}
#define IP_ADDRESS          192.168.1.100
#define GATEWAY_ADDRESS     192.168.1.1
#define NETMASK             255.255.255.0
```

### 4. 烧写到 MCU

```bash
# 使用 ST-Link 烧写
st-flash write build/stm32h7_zenoh.bin 0x08000000

# 或在 STM32CubeIDE 中按 Run
```

### 5. 验证

串口（115200 baud）应显示：

```
[INFO] STM32H7 Zenoh-Pico Demo
[INFO] Initializing Ethernet...
[INFO] IP: 192.168.1.100
[INFO] Connecting to Zenoh router...
[INFO] ✓ Connected
[INFO] Starting sensor acquisition at 1kHz
```

## 文件说明

```
├── src/
│   ├── main.c                   # 入口点
│   ├── config.h                 # 系统配置
│   ├── eth_driver.c             # 以太网驱动
│   ├── zenoh_client.c           # Zenoh-Pico 客户端
│   ├── realtime_scheduler.c     # 实时任务调度
│   ├── ptp_client.c             # PTP 客户端（预留）
│   └── sensor_interface.c       # 传感器接口
├── CMakeLists.txt               # 构建脚本
├── arm-toolchain.cmake          # ARM 编译工具链
└── README.md                    # 本文件
```

## 性能特性

### 实时性

- **任务调度**：FreeRTOS 提供软实时保证
- **中断延迟**：<5µs（裸机情况下）
- **消息处理**：<100µs（从网络到应用）

### 性能指标

| 指标 | 值 |
|------|-----|
| CPU | STM32H743 @ 480MHz |
| RAM | 512KB DTCM |
| Flash | 2MB Internal |
| 以太网 | 100Mbps（内置 MAC） |
| Zenoh 消息速率 | >1000 msg/sec |

## 网络拓扑

```
                    Zenoh Router
                        │
            ┌───────────┼────────────┐
            │           │            │
        STM32H7_1   STM32H7_2   STM32H7_3
        (Motor)     (Sensor)    (Encoder)
         PWM        ADC/I2C     SPI
```

## 时间同步（PTP）

阶段一未启用 PTP，但架构已预留了接口。在 `ptp_client.c` 中实现：

```c
// 初始化 PTP 客户端
ptp_init();

// 同步时钟（1kHz）
ptp_sync_clock();

// 获取本地时间戳（<10µs 精度）
uint64_t timestamp = ptp_get_timestamp();
```

## 扩展功能

### ADC 多通道采样

```c
// 在 src/sensor_interface.c 中配置
adc_config_t config = {
    .channels = {ADC_CH0, ADC_CH1, ADC_CH2},
    .sampling_rate = 1000,  // 1kHz
};
```

### CAN 总线集成

对于 CAN 设备（如电机驱动器）：

```c
can_init(CAN1, 1000000);  // 1Mbps
can_send(msg_id, data, length);
```

### SPI 高速数据采集

```c
spi_init(SPI1, 10000000);  // 10MHz
spi_read(buffer, length);
```

## 故障排除

### Q1: OpenOCD 无法识别设备

```bash
# 检查 ST-Link 驱动
lsusb | grep STMicroelectronics

# 重启调试器
openocd -f interface/stlink.cfg -f target/stm32h7x.cfg
```

### Q2: 以太网连接失败

```c
// 在 src/eth_driver.c 中启用调试
#define ETH_DEBUG 1

// 检查 PHY 芯片是否响应
if (!eth_phy_detect()) {
    printf("ERROR: PHY not found\n");
}
```

### Q3: 实时性不足

```c
// 检查任务优先级和堆栈大小
configMAX_PRIORITIES  // 增大优先级数
configMINIMAL_STACK_SIZE  // 增大栈大小
```

## 与 Zenoh Router 通信

### 从 Linux PC 发送命令

```bash
# PC 端（带 Zenoh Router）
zenoh pub "rt/stm32h7_1/servo/cmd" '{"angle": 90}'

# STM32H7 会通过订阅收到并执行
```

### 从 STM32H7 发布数据

```c
// STM32 固件中
zenoh_publish("rt/stm32h7_1/sensor/adc", adc_data);

// PC 端接收
zenoh sub "rt/stm32h7_1/sensor/adc"
```

## 进阶：运行时间敏感代码

对于需要<100µs 响应时间的代码，建议使用裸机中断：

```c
// 绕过 FreeRTOS，直接中断处理
void TIM1_UP_IRQHandler(void) {
    // <5µs 执行时间
    motor_update_pwm();
    TIM1->SR &= ~TIM_SR_UIF;
}
```

## 参考资源

- [STM32H7 参考手册](https://www.st.com/resource/en/reference_manual/dm00318382-stm32h745-stm32h747-stm32h755-and-stm32h757-advanced-arm-based-32-bit-mcus-stmicroelectronics.pdf)
- [Zenoh-Pico C API](https://github.com/eclipse-zenoh/zenoh-pico)
- [FreeRTOS 官方文档](https://www.freertos.org/Documentation/RTOS_book.html)

---

**维护者**：OpenNeuro 团队  
**最后更新**：2026 年 1 月

