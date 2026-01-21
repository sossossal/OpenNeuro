# TSN Scheduler - IEEE 802.1 时间感知网络调度

## 📋 项目概述

IEEE 802.1 时间感知调度系统 (Time-Aware Scheduling) 的完整实现，支持确定性网络延迟和优先级调度。

## 🎯 目标

- ✅ IEEE 802.1 Qbv 时间感知调度实现
- ✅ <1ms 确定性延迟保证
- ✅ 支持 Linux tc/taprio 驱动
- ✅ 硬件交换机集成支持
- ✅ 完整的流量优先级管理
- ✅ 与 PTP 无缝集成

## 📁 项目结构

```
tsn-scheduler/
├── src/
│   ├── core/
│   │   ├── tsn_scheduler.c          # TSN 主调度器
│   │   ├── traffic_class.c          # 流量分类
│   │   ├── gating_control.c         # 门控控制
│   │   └── bandwidth_manager.c      # 带宽管理
│   ├── platform/
│   │   ├── linux/
│   │   │   ├── taprio_interface.c   # tc/taprio 驱动接口
│   │   │   └── ethtool_config.c     # 以太网配置
│   │   └── switch/
│   │       ├── generic_switch.c     # 通用交换机接口
│   │       └── marvell_switch.c     # Marvell 交换机适配
│   ├── api/
│   │   ├── tsn.h                    # 公共 API 接口
│   │   └── tsn_types.h              # 数据结构定义
│   └── util/
│       ├── schedule_calc.c          # 调度计算算法
│       └── validation.c             # 合法性验证
├── test/
│   ├── unit_test.c
│   ├── traffic_test.c               # 流量生成测试
│   └── latency_test.c               # 延迟测试
├── doc/
│   ├── ARCHITECTURE.md
│   ├── ALGORITHM.md                 # 调度算法说明
│   └── HARDWARE_SUPPORT.md          # 硬件支持清单
├── example/
│   ├── simple_schedule.c            # 简单调度示例
│   ├── multimedia_schedule.c        # 多媒体调度示例
│   └── industrial_schedule.c        # 工业控制调度示例
├── CMakeLists.txt
├── .gitignore
└── README.md

```

## 🚀 快速开始

### 在 Linux 上配置 TSN

```bash
# 编译库
cd tsn-scheduler
mkdir build && cd build
cmake ..
make

# 使用示例
./example/simple_schedule
```

### 基本用法

```c
#include "tsn.h"

int main() {
    // 创建 TSN 调度器
    tsn_scheduler_t *sched = tsn_scheduler_create();
    
    // 定义流量类 (4 个优先级)
    tsn_traffic_class_t tc[4] = {
        {.priority = 0, .bandwidth = 100},  // 实时控制
        {.priority = 1, .bandwidth = 200},  // 视频流
        {.priority = 2, .bandwidth = 150},  // 音频流
        {.priority = 3, .bandwidth = 50}    // 最佳努力
    };
    
    // 配置调度
    tsn_scheduler_config(sched, tc, 4);
    
    // 启用调度
    tsn_scheduler_enable(sched);
    
    return 0;
}
```

## 📊 性能指标

| 指标 | 目标 | 进度 |
|------|------|------|
| 确定性延迟 | <1ms | 📅 W3-W9 |
| 调度开销 | <10µs | 📅 W8 |
| 支持优先级数 | 4-8 个 | 📅 W5 |
| 内存占用 | <128KB | 📅 W6 |
| 代码行数 | ~3000 | 📅 W15 |

## 🗓️ 开发计划

- **W1-W2**: 需求分析与架构设计
- **W3-W4**: 流量分类与算法实现
- **W5-W6**: 门控控制实现
- **W7-W8**: Linux tc/taprio 驱动集成
- **W9**: 硬件交换机支持
- **W10-W12**: 完整系统集成与测试
- **W13-W15**: 文档与优化

## 🔗 依赖

- Linux 内核 5.15+ (tc/taprio 支持)
- libnl (Netlink 接口)
- PTP 同步 (从 ptp-stack 提供)

## 📈 算法说明

TSN 调度器使用 **时间驱动的门控算法** (Time-Driven Gate Control):

1. **周期调度**: 以固定时间周期 (如 1ms) 为单位
2. **优先级门控**: 每个时间段开放特定优先级的门
3. **带宽保证**: 为每个流量类预留固定带宽
4. **抖动控制**: 最小化数据包延迟变动

详见 [ALGORITHM.md](doc/ALGORITHM.md)

## 📝 许可证

Apache License 2.0

---

**创建日期**: 2026年1月20日  
**当前阶段**: 需求分析 (W1-W2)  
**下一步**: 完成架构设计 (2026年2月9日)
