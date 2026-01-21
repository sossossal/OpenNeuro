# Hello World Demo: 用 ROS 2 控制 Zenoh-Pico 舵机

## 概述

本 Demo 演示了完整的 OpenNeuro 工作流：
1. ESP32 上的 Zenoh-Pico 客户端
2. ROS 2 ↔ Zenoh 桥接器
3. ROS 2 命令行控制舵机

## 场景

```
你的 PC
  │
  ├─→ ROS 2 CLI / rviz
  │
  └─→ Zenoh Router + Bridge
        │
        └─→ WiFi / 以太网
                │
                └─→ ESP32 + 舵机
                    (连接到 GPIO 18 PWM)
```

## 快速开始

### 第 1 步：准备硬件

1. ESP32 开发板
2. PWM 舵机（如 SG90）
3. USB 转串口线（烧写和调试）
4. WiFi 接入点

**接线**：
```
ESP32 GPIO 18 (PWM) → 舵机 Signal
ESP32 GND           → 舵机 GND
ESP32 5V（通过降压） → 舵机 VCC
```

### 第 2 步：烧写 ESP32 固件

```bash
cd ../zenoh-pico-esp32

# 编辑 platformio.ini，配置你的 WiFi SSID 和密码
# 修改:
#   WIFI_SSID=Your_WiFi_SSID
#   WIFI_PASSWORD=Your_WiFi_Password

# 烧写
pio run -t upload -e esp32-s3-devkitc-1 --upload-port COM3

# 监控（保持运行）
pio device monitor --port COM3 --baud 115200
```

预期输出：
```
[INFO] WiFi connected, IP: 192.168.1.100
[INFO] Connected to Zenoh router at tcp/192.168.1.1:7447
[INFO] Servo initialized on GPIO 18
```

### 第 3 步：启动 Zenoh Router（新终端）

```bash
# 确保安装了 Zenoh
cargo install zenoh-cli

# 启动路由器
zenohd -l tcp/0.0.0.0:7447
```

预期输出：
```
2026-01-20 10:00:00 - INFO - Zenoh router initialized
```

### 第 4 步：启动 ROS 2 ↔ Zenoh 桥接器（新终端）

```bash
cd bridge

# 编译 Rust 桥接器
cargo run --release

# 或使用 Python 版本（如果已实现）
python3 ros2_zenoh_bridge.py
```

预期输出：
```
[INFO] OpenNeuro ROS 2 ↔ Zenoh Bridge v0.1.0
[INFO] ✓ Connected to Zenoh
[INFO] Publishers and Subscribers initialized
```

### 第 5 步：控制舵机（新终端）

#### 方式 1：使用 Zenoh CLI（最直接）

```bash
# 发送舵机目标角度（0-180度）
echo '{"angle": 90}' | zenoh pub "rt/device/servo/cmd" -

# 你应该看到舵机转向 90 度中位

# 尝试其他角度
echo '{"angle": 0}' | zenoh pub "rt/device/servo/cmd" -   # 转向 0 度
echo '{"angle": 180}' | zenoh pub "rt/device/servo/cmd" - # 转向 180 度
```

#### 方式 2：使用 ROS 2 CLI（如果安装了 ROS 2）

```bash
# 新终端：订阅舵机反馈
ros2 topic echo /servo/feedback

# 发送控制命令
ros2 topic pub /servo/target geometry_msgs/msg/Vector3 "{x: 90.0, y: 0.0, z: 0.0}" --once
```

#### 方式 3：查看实时反馈

```bash
# 订阅舵机的当前状态
zenoh sub "rt/device/servo/feedback"

# 你将看到类似输出
[SUBSCRIBER] received ('rt/device/servo/feedback', 
  '{"current_angle": 90.0, "temperature": 35.5, "timestamp_ms": 1500}')
```

## 性能验证

### 延迟测试

```bash
# 记录发送时间和反馈时间
echo '{"angle": 45}' | zenoh pub "rt/device/servo/cmd" - --wait-liveliness
zenoh sub "rt/device/servo/feedback"

# 典型延迟：<2ms（WiFi）或 <0.5ms（以太网）
```

### 吞吐量测试

```bash
# 以 100Hz 频率发送命令
for i in {1..100}; do
    angle=$((i % 180))
    echo "{\"angle\": $angle}" | zenoh pub "rt/device/servo/cmd" -
    sleep 0.01  # 100Hz = 10ms 间隔
done

# 观察舵机是否平稳扫动
```

## 故障排除

### 问题 1：ESP32 无法连接 WiFi

**症状**：
```
[ERROR] WiFi connection failed
```

**解决**：
- 检查 WiFi SSID 和密码是否正确
- 确保 WiFi 支持 2.4GHz（ESP32 不支持 5GHz）
- 检查距离和信号强度

### 问题 2：无法连接到 Zenoh Router

**症状**：
```
[ERROR] Failed to connect to tcp/192.168.1.1:7447
```

**解决**：
```bash
# 检查 Router 是否正在运行
netstat -an | grep 7447

# 从 ESP32 ping Router
# （在 ESP32 代码中添加 ping 测试）

# 检查防火墙
# Windows: 允许 zenohd.exe 通过防火墙
# Linux: sudo ufw allow 7447
```

### 问题 3：舵机不转动

**症状**：
```
[INFO] Servo command received: angle = 90
// 但舵机没反应
```

**解决**：
- 检查舵机接线（GPIO 18 PWM 是否连接）
- 检查舵机电源（通常需要 5-6V）
- 在 ESP32 代码中测试 PWM 输出（不通过 Zenoh）

### 问题 4：桥接器无法启动

**症状**：
```
error[E0433]: cannot find `zenoh` in this scope
```

**解决**：
```bash
cd bridge
cargo build --release  # 确保依赖下载完整
cargo run --release
```

## 进阶：自定义舵机控制

### 调整舵机速度和加速度

编辑 `firmware/zenoh-pico-esp32/src/servo_controller.c`：

```c
typedef struct {
    float angle;           // 目标角度
    uint16_t speed;        // 速度（0-255）
    uint16_t acceleration; // 加速度
} servo_cmd_t;

// 处理消息
servo_cmd_t cmd = parse_servo_command(payload);
servo_set_speed(cmd.speed);
servo_goto_angle(cmd.angle);
```

### 添加多个舵机

```c
// servo_controller.c
#define NUM_SERVOS 4

for (int i = 0; i < NUM_SERVOS; i++) {
    servo_init(gpio_pins[i]);
    zenoh_subscribe(topics[i]);  // rt/device/servo_0/cmd, servo_1/cmd, ...
}
```

### 与 ROS 2 集成的完整工作流

```bash
# 1. 启动所有服务
zenohd &
cargo run --release --manifest-path bridge/Cargo.toml &

# 2. 在 ROS 2 中定义舵机节点
cd ../rospackages
ros2 launch servo_controller servo_demo.launch.py

# 3. 在 rviz 中可视化和控制
rviz2 -d servo_controller.rviz
```

## 下一步

完成本 Demo 后，建议：

1. ✓ **基础理解**：修改舵机角度范围、速度等参数
2. ✓ **多轴控制**：扩展到 2 个或 4 个舵机
3. ✓ **传感器集成**：添加关节角度反馈（编码器）
4. ✓ **实时性升级**：使用 STM32H7 + PTP 实现 <10µs 同步（阶段二）

## 参考代码

- [ESP32 Servo 控制驱动](../zenoh-pico-esp32/src/servo_controller.c)
- [Zenoh 发布订阅示例](https://github.com/eclipse-zenoh/zenoh-rust-examples)
- [ROS 2 Custom Message 教程](https://docs.ros.org/en/jazzy/Tutorials/Advanced/Custom-RclCpp-Msg-Type-Publisher-Subscriber.html)

---

**难度等级**：⭐⭐ 初级  
**预计时间**：1-2 小时  
**最后更新**：2026 年 1 月

