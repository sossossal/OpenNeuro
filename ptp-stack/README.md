# PTP Stack - IEEE 1588 v2 时间同步协议栈

## 📋 项目概述

完整的 IEEE 1588 v2 (PTP - Precision Time Protocol) 协议栈实现，支持多平台部署，目标精度 <10µs。

## 🎯 目标

- ✅ IEEE 1588 v2 完整协议栈实现
- ✅ Master/Slave/Transparent 模式支持
- ✅ <10µs 时间同步精度 (目标)
- ✅ 多平台支持 (STM32H7, RK3588, ESP32)
- ✅ 零拷贝设计，最小化延迟
- ✅ 完整的测试套件和文档

## 📁 项目结构

```
ptp-stack/
├── src/
│   ├── core/
│   │   ├── ptp_main.c           # PTP 主程序框架
│   │   ├── ptp_state_machine.c  # Master/Slave 状态机
│   │   ├── ptp_messages.c       # PTP 消息处理
│   │   └── servo.c              # PI Servo 时钟控制
│   ├── platform/
│   │   ├── stm32h7/
│   │   │   ├── timestamp_driver.c    # 硬件时间戳驱动
│   │   │   ├── network_driver.c      # 网络驱动
│   │   │   └── config.h
│   │   ├── rk3588/
│   │   │   ├── timestamp_driver.c
│   │   │   └── config.h
│   │   └── esp32/
│   │       ├── timestamp_driver.c
│   │       └── config.h
│   ├── api/
│   │   ├── ptp.h                # 公共 API 接口
│   │   └── ptp_types.h          # 数据结构定义
│   └── util/
│       ├── logging.c            # 日志系统
│       └── crc.c                # CRC 校验
├── test/
│   ├── unit_test.c              # 单元测试
│   ├── integration_test.c        # 集成测试
│   └── performance_test.c        # 性能测试
├── doc/
│   ├── ARCHITECTURE.md           # 架构设计文档
│   ├── API_REFERENCE.md          # API 文档
│   └── DESIGN_DECISIONS.md       # 设计决策记录
├── example/
│   ├── master_example.c          # Master 模式示例
│   ├── slave_example.c           # Slave 模式示例
│   └── measurement_example.c     # 精度测量示例
├── CMakeLists.txt
├── .gitignore
└── README.md

```

## 🚀 快速开始

### 构建 (STM32H7)
```bash
cd ptp-stack
mkdir build && cd build
cmake -DPLATFORM=STM32H7 ..
make
```

### 运行测试
```bash
cd ptp-stack
mkdir build && cd build
cmake -DBUILD_TESTS=ON ..
make test
```

## 📊 性能指标

| 指标 | 目标 | 进度 |
|------|------|------|
| 同步精度 | <10µs | 📅 W5-W10 |
| 消息处理延迟 | <100µs | 📅 W5-W10 |
| 吞吐量 | >1000 msg/sec | 📅 W10 |
| 内存占用 | <256KB | 📅 W5 |
| 代码行数 | ~5000 | 📅 W20 |

## 🗓️ 开发计划

- **W1-W4**: 架构设计与 API 定义
- **W5-W7**: Master 消息处理实现
- **W8-W9**: Slave 时钟同步实现
- **W10**: PI Servo 算法实现
- **W11-W14**: 跨平台适配 (STM32H7, RK3588, ESP32)
- **W15-W16**: 集成测试与性能验证
- **W17-W20**: 文档与示例代码完成
- **W21**: Alpha 发布

## 🔗 依赖

- FreeRTOS / Zephyr RTOS
- LwIP (网络协议栈)
- CMake 3.10+
- GCC/Clang 编译器

## 📝 许可证

Apache License 2.0

## 🤝 贡献

欢迎提交 Issue 和 Pull Request！

---

**创建日期**: 2026年1月20日  
**当前阶段**: 架构设计 (W1-W4)  
**下一步**: 完成架构评审 (2026年2月2日)
