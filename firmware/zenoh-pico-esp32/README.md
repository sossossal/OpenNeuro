# Zenoh-Pico ESP32 移植

## 概述

本目录包含 Zenoh-Pico 在 ESP32 平台上的完整移植，支持 WiFi 和以太网两种连接方式。

## 硬件要求

- **ESP32 开发板**：ESP32-S3-DevKitC-1（推荐）或 ESP32-S3-DevKitM-1
- **USB 转串口**：用于烧写和调试（通常板子自带）
- **WiFi**：2.4GHz WiFi 接入点（SSID + 密码）

## 快速开始

### 1. 编译与烧写

#### 使用 PlatformIO（推荐）

```bash
# 安装依赖
pio platform install espressif32
pio lib install "Zenoh-Pico"

# 编译
pio run -e esp32-s3-devkitc-1

# 烧写到 COM3（根据实际修改）
pio run -t upload -e esp32-s3-devkitc-1 --upload-port COM3

# 监控串口
pio device monitor --port COM3 --baud 115200
```

#### 使用 ESP-IDF

```bash
# 配置 WiFi 等参数
idf.py menuconfig

# 编译
idf.py build

# 烧写
idf.py -p COM3 flash monitor
```

### 2. 配置 WiFi

编辑 `platformio.ini` 或 `sdkconfig`，填入你的 WiFi 信息：

```ini
[platformio]
...
build_flags = 
    -D WIFI_SSID="Your_WiFi_SSID"
    -D WIFI_PASSWORD="Your_WiFi_Password"
```

### 3. 验证连接

烧写完成后，监控串口应看到：

```
[INFO] WiFi connecting to "Your_WiFi_SSID"...
[INFO] WiFi connected, IP: 192.168.1.100
[INFO] Connecting to Zenoh router at tcp/192.168.1.1:7447...
[INFO] ✓ Connected to Zenoh
[INFO] Publishing sensor data every 100ms...
[SENSOR] Temperature: 28.5°C, Humidity: 45.2%
```

## 文件说明

```
├── src/
│   ├── main.c              # 入口点
│   ├── wifi_config.h       # WiFi 配置
│   ├── zenoh_client.c      # Zenoh-Pico 客户端实现
│   ├── sensor_driver.c     # 传感器驱动（DHT11 示例）
│   └── servo_controller.c  # PWM 舵机控制
├── platformio.ini          # PlatformIO 配置
├── CMakeLists.txt          # ESP-IDF 构建配置
└── README.md               # 本文件
```

## 功能说明

### 发布者（Publisher）

本固件定期发布传感器数据：

```c
// 发布主题
rt/device/esp32_001/sensor/temperature  // 温度
rt/device/esp32_001/sensor/humidity     // 湿度
rt/device/esp32_001/servo/status        // 舵机状态
```

### 订阅者（Subscriber）

本固件监听来自上位机的控制指令：

```c
// 订阅主题
rt/device/esp32_001/servo/cmd          // 舵机目标角度
rt/device/esp32_001/gpio/output        // GPIO 输出控制
```

### 示例消息格式

**温度发布**：
```json
{
    "temperature": 28.5,
    "unit": "celsius",
    "timestamp_ms": 12345
}
```

**舵机控制指令**：
```json
{
    "angle": 90.0,
    "speed": 100,
    "acceleration": 50
}
```

## 性能指标

| 指标 | 值 |
|------|-----|
| 发布频率 | 10Hz |
| 平均延迟 | 50-150ms（WiFi）|
| 内存占用 | ~400KB RAM, 1.2MB Flash |
| 功耗 | 80-120mA（WiFi active） |

## 故障排除

### Q1: 无法连接到 WiFi

```c
// 检查 WiFi 凭证是否正确
// 查看串口输出中的具体错误信息
```

### Q2: 无法连接到 Zenoh Router

```bash
# 确保 Router 正在运行
zenohd

# 检查网络连通性
ping <router_ip>

# 检查防火墙是否开放端口 7447
```

### Q3: 程序崩溃

```
启用调试模式：
- platformio.ini 中添加: build_flags = -v -g
- 使用 espressif/esp-idf:latest 容器
```

## 扩展功能

### 添加新传感器

在 `sensor_driver.c` 中实现 `sensor_init()` 和 `sensor_read()`：

```c
void sensor_init() {
    // 初始化 I2C/SPI 等
}

sensor_data_t sensor_read() {
    // 读取传感器数据
    return data;
}
```

### 添加舵机控制

在 `servo_controller.c` 中实现 PWM 控制逻辑

### 集成时间同步（阶段二）

预留了 PTP 客户端接口，可在阶段二实现 IEEE 1588 时间同步

## 参考资源

- [ESP-IDF 官方文档](https://docs.espressif.com/projects/esp-idf/en/latest/)
- [Zenoh-Pico GitHub](https://github.com/eclipse-zenoh/zenoh-pico)
- [PlatformIO 文档](https://docs.platformio.org/)

---

**维护者**：OpenNeuro 团队  
**最后更新**：2026 年 1 月

