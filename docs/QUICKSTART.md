# OpenNeuro 快速开始指南

## 第一步：环境准备

### 1.1 Python 环境（PC 端工具）

```bash
# 创建虚拟环境
python -m venv venv
source venv/Scripts/activate  # Windows: venv\Scripts\activate

# 安装依赖
pip install zenoh paho-mqtt
```

### 1.2 Rust 环境（编译 Zenoh Router）

```bash
# 安装 Rust（如果还未安装）
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh

# 安装 Zenoh CLI
cargo install zenoh-cli
```

### 1.3 嵌入式工具链

#### 对于 ESP32：
```bash
# 安装 ESP-IDF
git clone https://github.com/espressif/esp-idf.git
cd esp-idf
./install.bat  # Windows

# 或使用 PlatformIO（推荐新手）
pip install platformio
pio platform install espressif32
```

#### 对于 STM32：
```bash
# 安装 ARM GCC
# 下载: https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads

# 或使用 STM32CubeIDE（集成开发环境）
# 下载: https://www.st.com/en/development-tools/stm32cubeide.html
```

---

## 第二步：启动 Zenoh Router

Zenoh Router 是通信的核心枢纽，所有设备都通过它进行连接。

```bash
# 启动路由器（默认监听 127.0.0.1:7447）
zenohd

# 如果需要监听所有网卡
zenohd -l "tcp/0.0.0.0:7447"
```

**验证**：看到类似输出说明成功：
```
2026-01-20 10:00:00 - INFO - Loading new config: ...
2026-01-20 10:00:00 - INFO - Zenoh router initialized
```

---

## 第三步：烧写固件到 ESP32

### 选项 A：使用 PlatformIO（推荐快速开始）

```bash
cd firmware/zenoh-pico-esp32

# 查看可用板子
pio boards | grep esp32

# 配置 platformio.ini
# 编辑文件，修改 [env:esp32-s3-devkitc-1] 部分

# 编译并烧写
pio run --target upload --upload-port COM3
# Windows COM3 可能需要替换为你的实际串口

# 查看串口输出
pio device monitor --port COM3 --baud 115200
```

### 选项 B：使用 ESP-IDF（更多控制）

```bash
cd firmware/zenoh-pico-esp32

# 配置
idf.py menuconfig

# 编译
idf.py build

# 烧写
idf.py -p COM3 flash

# 监控
idf.py -p COM3 monitor
```

---

## 第四步：验证设备连接

在 PC 端，使用 Zenoh CLI 订阅设备消息：

```bash
# 订阅 ESP32 发布的传感器数据
zenoh sub "rt/device/esp32_001/sensor/temperature"

# 发布控制指令给 ESP32
echo '{"angle": 90}' | zenoh pub "rt/device/esp32_001/servo/cmd" -

```

**预期输出**：
```
[SUBSCRIBER] received ('rt/device/esp32_001/sensor/temperature', '{"temp": 28.5}')
```

---

## 第五步：运行 Hello World Demo

### 场景：用 ROS 2 控制 ESP32 上的舵机

#### 5.1 启动 ROS 2 环境

```bash
# 新开一个终端

# 如果安装了 ROS 2 Jazzy
source /opt/ros/jazzy/setup.bash

# 启动 ROS 2 bridge
cd bridge
python3 ros2_zenoh_bridge.py
```

#### 5.2 发送控制命令

```bash
# 再新开一个终端

# 发布舵机目标角度
ros2 topic pub /servo/target std_msgs/msg/Float32 'data: 90.0' --once

# 订阅舵机当前角度
ros2 topic echo /servo/feedback
```

#### 5.3 查看 ESP32 响应

在之前的 PlatformIO monitor 中应该看到：
```
[INFO] Servo command received: angle = 90 degrees
[INFO] Moving servo...
[INFO] Servo moved, current angle: 90 degrees
```

---

## 第六步：性能测试（验证延迟）

```bash
cd tests/latency_tests

# 测试 P2P 延迟
python3 latency_test.py

# 预期结果
# P2P Latency (ESP32 over WiFi): 1.8ms (平均)
# 99th percentile: 2.4ms
```

---

## 常见问题排查

### Q1: 无法连接到 Zenoh Router

```bash
# 检查 Router 是否运行
netstat -an | grep 7447

# 检查防火墙设置
# Windows: 允许 zenohd.exe 通过防火墙
```

### Q2: ESP32 编译失败

```bash
# 清除构建缓存
pio run --target clean

# 重新编译
pio run
```

### Q3: 串口识别失败

```bash
# Windows: 查看串口列表
mode

# Linux: 查看串口
ls /dev/ttyUSB*

# macOS: 查看串口
ls /dev/cu.usbserial*
```

### Q4: ROS 2 找不到话题

```bash
# 检查 ROS 2 bridge 是否运行
ros2 node list

# 检查话题列表
ros2 topic list
```

---

## 下一步

完成快速开始后，建议：

1. 📖 阅读 [ARCHITECTURE.md](ARCHITECTURE.md) 了解系统设计
2. 💻 探索 [demos/](../demos/) 中的其他示例
3. 🧪 运行 [tests/](../tests/) 中的测试套件
4. 🔧 尝试修改固件代码，自定义传感器/执行器

---

## 获取帮助

- 📌 GitHub Issues: [提交问题](https://github.com/yourusername/OpenNeuro/issues)
- 💬 讨论区: [社区讨论](https://github.com/yourusername/OpenNeuro/discussions)
- 📚 官方文档: [Zenoh 文档](https://zenoh.io/docs/), [ROS 2 文档](https://docs.ros.org/)

