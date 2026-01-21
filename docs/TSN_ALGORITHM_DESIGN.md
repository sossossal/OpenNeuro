# TSN 调度器算法详设

**版本**: v0.2.0  
**日期**: 2026-01-20  
**阶段**: Stage 2 Week 2  
**目标**: IEEE 802.1Qbv 时间感知门控调度完整算法设计

---

## 📋 目录

1. [算法概述](#算法概述)
2. [流量分类与优先级](#流量分类与优先级)
3. [门控调度算法](#门控调度算法)
4. [带宽预留机制](#带宽预留机制)
5. [数据结构设计](#数据结构设计)
6. [实现框架](#实现框架)
7. [Linux 集成](#linux-集成)
8. [W2-W4 开发计划](#w2-w4-开发计划)

---

## 算法概述

### 核心思想

IEEE 802.1Qbv 时间感知调度 (Time-Aware Scheduling) 在以太网交换机和端口层实现**确定性数据包转发**：

```
输入流
  │
  ├─ 流 1 (RT: Real-Time)    ─→ [优先级 7-6] ─┐
  ├─ 流 2 (ST: Streaming)    ─→ [优先级 5-4] ─┤
  ├─ 流 3 (BE: Best-Effort) ─→ [优先级 3-0] ─┤ 门控调度
  │                                           │ (Time-Aware)
  │                                           │
输出端口 ◄──── 时间同步 (PTP) 精确时间基准 ◄─┘
       ┌─────────────────────────┐
       │ 时间周期: 1ms
       │ ├─ 0-100µs: RT 门打开
       │ ├─ 100-300µs: ST 门打开
       │ └─ 300-1000µs: BE 门打开
       └─────────────────────────┘
```

### 关键指标

| 指标 | 目标 | 实现难度 |
|------|------|--------|
| RT 延迟 | <100µs | ★★★★ |
| ST 延迟 | <500µs | ★★★ |
| 控制延迟 | <1ms | ★★ |
| 数据包乱序 | 不允许 | ★★★★★ |
| 丢包率 | <0.1% | ★★★ |

---

## 流量分类与优先级

### 定义 4 个流量类别

```c
/**
 * IEEE 802.1Qbv 定义了 8 个优先级 (0-7)
 * 我们分为 4 个流量类别:
 */
enum tsn_traffic_class {
    TSN_CLASS_RT = 0,        // Real-Time: 优先级 7-6 (周期 <100µs)
    TSN_CLASS_ST = 1,        // Streaming: 优先级 5-4 (周期 100µs-1ms)
    TSN_CLASS_CT = 2,        // Control: 优先级 3-2 (周期 1-100ms)
    TSN_CLASS_BE = 3         // Best-Effort: 优先级 1-0 (无保证)
};

/**
 * 流量识别规则
 */
typedef struct {
    uint8_t vlan_id;          // VLAN ID
    uint8_t priority;         // 802.1p 优先级 (0-7)
    uint32_t src_ip;
    uint32_t dst_ip;
    uint16_t src_port;
    uint16_t dst_port;
    uint8_t protocol;         // TCP/UDP/ICMP
} tsn_flow_identifier_t;

/**
 * 流属性
 */
typedef struct {
    uint32_t flow_id;
    tsn_traffic_class traffic_class;
    uint32_t max_frame_size;   // 最大帧长 (bytes)
    uint32_t max_bandwidth;    // 最大带宽 (Mbps)
    uint32_t period_us;        // 周期 (µs)
    uint32_t deadline_us;      // 截止时间 (µs)
} tsn_flow_properties_t;
```

### 映射表

```
流量类别          优先级    发送门控    最大延迟    应用例
────────────────┬────────┬─────────┬──────────┬─────────────────
Real-Time (RT) │ 7, 6   │ 必须    │ <100µs   │ 工业控制、协作机器人
Streaming (ST) │ 5, 4   │ 可选    │ <500µs   │ 实时视频、音频
Control (CT)   │ 3, 2   │ 可选    │ <1ms     │ 系统管理、诊断
Best-Effort    │ 1, 0   │ 否      │ 无保证   │ 互联网流量
(BE)           │        │         │          │
```

---

## 门控调度算法

### 时间周期设计

```c
/**
 * 门控周期: 1 毫秒 (1000 微秒)
 * 可分为 4 个时间槽:
 */

#define TSN_CYCLE_PERIOD_US  1000   // 1ms

typedef struct {
    // 时间槽分配
    uint32_t rt_slot_start;    // 0µs
    uint32_t rt_slot_end;      // 100µs (RT 优先级 7-6)
    
    uint32_t st_slot_start;    // 100µs
    uint32_t st_slot_end;      // 400µs (ST 优先级 5-4)
    
    uint32_t ct_slot_start;    // 400µs
    uint32_t ct_slot_end;      // 900µs (CT 优先级 3-2)
    
    uint32_t be_slot_start;    // 900µs
    uint32_t be_slot_end;      // 1000µs (BE 优先级 1-0)
    
} tsn_cycle_config_t;

// 默认配置
tsn_cycle_config_t tsn_default_cycle = {
    .rt_slot_start = 0,
    .rt_slot_end = 100,
    .st_slot_start = 100,
    .st_slot_end = 400,
    .ct_slot_start = 400,
    .ct_slot_end = 900,
    .be_slot_start = 900,
    .be_slot_end = 1000,
};
```

### 门控状态机

```
时刻 t (mod 1000µs)
│
├─ t ∈ [0, 100µs)      ─→ RT 门: OPEN
│                          ST 门: CLOSE
│                          CT 门: CLOSE
│                          BE 门: CLOSE
│
├─ t ∈ [100µs, 400µs) ─→ RT 门: CLOSE
│                          ST 门: OPEN
│                          CT 门: CLOSE
│                          BE 门: CLOSE
│
├─ t ∈ [400µs, 900µs) ─→ RT 门: CLOSE
│                          ST 门: CLOSE
│                          CT 门: OPEN
│                          BE 门: CLOSE
│
└─ t ∈ [900µs, 1000µs)─→ RT 门: CLOSE
                           ST 门: CLOSE
                           CT 门: CLOSE
                           BE 门: OPEN
```

### 算法伪代码

```c
/**
 * 线程安全的门控决策
 * 在网络驱动中调用
 */
bool tsn_can_transmit_packet(const tsn_frame_t *frame) {
    // 1. 获取当前精确时间 (PTP 同步)
    uint64_t current_time_us = ptp_get_time_us();
    uint32_t cycle_offset_us = current_time_us % TSN_CYCLE_PERIOD_US;
    
    // 2. 判断帧的优先级
    uint8_t priority = frame->priority;
    uint8_t traffic_class = tsn_priority_to_class(priority);
    
    // 3. 检查当前时间槽是否允许该优先级
    switch (traffic_class) {
        case TSN_CLASS_RT:
            return (cycle_offset_us >= tsn_cycle.rt_slot_start &&
                    cycle_offset_us < tsn_cycle.rt_slot_end);
        
        case TSN_CLASS_ST:
            return (cycle_offset_us >= tsn_cycle.st_slot_start &&
                    cycle_offset_us < tsn_cycle.st_slot_end);
        
        case TSN_CLASS_CT:
            return (cycle_offset_us >= tsn_cycle.ct_slot_start &&
                    cycle_offset_us < tsn_cycle.ct_slot_end);
        
        case TSN_CLASS_BE:
            // BE 流量总是允许发送 (未被更高优先级占用时)
            return true;
        
        default:
            return false;
    }
}

/**
 * 主网络驱动循环
 * 每 10µs 调用一次
 */
void tsn_packet_scheduler_tick(void) {
    // 遍历所有待发送队列
    for (int i = 7; i >= 0; i--) {  // 高优先级优先
        tsn_frame_t *frame;
        
        while ((frame = tsn_queue_peek(&txq[i])) != NULL) {
            if (tsn_can_transmit_packet(frame)) {
                // 发送该帧
                tsn_queue_dequeue(&txq[i]);
                ptp_eth_transmit(frame);
                break;  // 该周期只发送一个帧，避免优先级反转
            } else {
                break;  // 当前优先级不允许，跳过
            }
        }
    }
}
```

---

## 带宽预留机制

### 基于优先级的带宽分配

```c
/**
 * 每个流量类别的最大带宽配置
 */
typedef struct {
    uint32_t max_bandwidth_mbps;  // 最大带宽 Mbps
    uint32_t current_usage_bps;   // 当前使用 bps
    uint32_t peak_bandwidth_bps;  // 峰值带宽 bps
} tsn_bandwidth_config_t;

tsn_bandwidth_config_t tsn_bandwidth[] = {
    // RT: Real-Time
    {
        .max_bandwidth_mbps = 400,   // 40% of 1Gbps
        .current_usage_bps = 0,
        .peak_bandwidth_bps = 0
    },
    // ST: Streaming
    {
        .max_bandwidth_mbps = 300,   // 30% of 1Gbps
        .current_usage_bps = 0,
        .peak_bandwidth_bps = 0
    },
    // CT: Control
    {
        .max_bandwidth_mbps = 150,   // 15% of 1Gbps
        .current_usage_bps = 0,
        .peak_bandwidth_bps = 0
    },
    // BE: Best-Effort
    {
        .max_bandwidth_mbps = 150,   // 15% of 1Gbps
        .current_usage_bps = 0,
        .peak_bandwidth_bps = 0
    },
};

/**
 * 流量入场控制 (Admission Control)
 * 检查新流是否可以加入
 */
bool tsn_admission_control(const tsn_flow_properties_t *flow) {
    // 计算该流所需带宽
    uint32_t flow_bandwidth_bps = 
        (flow->max_frame_size * 8) * 1000000 / flow->period_us;
    
    tsn_traffic_class class = flow->traffic_class;
    uint32_t available_bandwidth = 
        tsn_bandwidth[class].max_bandwidth_mbps * 1000000 -
        tsn_bandwidth[class].current_usage_bps;
    
    if (flow_bandwidth_bps > available_bandwidth) {
        TSN_LOG_WARN("Admission denied: need %u bps, available %u bps",
                     flow_bandwidth_bps, available_bandwidth);
        return false;
    }
    
    // 接纳流量
    tsn_bandwidth[class].current_usage_bps += flow_bandwidth_bps;
    
    TSN_LOG_INFO("Flow %u admitted to class %d", 
                 flow->flow_id, flow->traffic_class);
    return true;
}

/**
 * 移除流量时释放带宽
 */
void tsn_remove_flow(uint32_t flow_id) {
    tsn_flow_properties_t *flow = tsn_find_flow(flow_id);
    if (flow == NULL) return;
    
    uint32_t flow_bandwidth_bps = 
        (flow->max_frame_size * 8) * 1000000 / flow->period_us;
    
    tsn_traffic_class class = flow->traffic_class;
    tsn_bandwidth[class].current_usage_bps -= flow_bandwidth_bps;
}
```

---

## 数据结构设计

### 队列结构

```c
/**
 * 优先级队列: 8 个队列对应 8 个优先级
 */
typedef struct {
    tsn_frame_t *head;
    tsn_frame_t *tail;
    uint32_t depth;
    uint32_t max_depth;
} tsn_priority_queue_t;

typedef struct {
    tsn_priority_queue_t queues[8];  // 优先级 7-0
} tsn_tx_queues_t;

/**
 * 帧描述符
 */
typedef struct tsn_frame {
    // 数据指针
    uint8_t *data;
    uint16_t length;
    
    // 时间信息
    uint64_t arrival_time_us;   // 到达时间
    uint64_t deadline_us;        // 截止时间
    
    // 优先级和分类
    uint8_t priority;            // 0-7
    uint8_t traffic_class;       // RT/ST/CT/BE
    uint32_t flow_id;
    
    // 链表指针
    struct tsn_frame *next;
    
} tsn_frame_t;
```

### 流表

```c
/**
 * 全局流表
 * 最多支持 256 个同时流
 */
#define TSN_MAX_FLOWS 256

typedef struct {
    uint32_t flow_id;
    tsn_flow_identifier_t identifier;
    tsn_flow_properties_t properties;
    
    // 统计信息
    uint64_t packets_transmitted;
    uint64_t packets_dropped;
    uint32_t max_latency_us;
    uint32_t avg_latency_us;
    
} tsn_flow_entry_t;

tsn_flow_entry_t tsn_flow_table[TSN_MAX_FLOWS];
uint32_t tsn_flow_count = 0;

/**
 * 查找流条目
 */
tsn_flow_entry_t* tsn_find_flow_entry(uint32_t flow_id) {
    for (int i = 0; i < tsn_flow_count; i++) {
        if (tsn_flow_table[i].flow_id == flow_id) {
            return &tsn_flow_table[i];
        }
    }
    return NULL;
}
```

---

## 实现框架

### 文件组织

```
tsn-scheduler/src/
├── core/
│  ├── tsn.h                    # TSN 核心头文件
│  ├── tsn.c                    # 核心初始化
│  ├── scheduler.h              # 调度器头文件
│  ├── scheduler.c              # 时间感知调度 (200+ LOC)
│  ├── queue.h                  # 队列管理
│  ├── queue.c                  # 队列实现 (80 LOC)
│  ├── flow.h                   # 流表管理
│  ├── flow.c                   # 流表实现 (100 LOC)
│  ├── bandwidth.h              # 带宽控制
│  └── bandwidth.c              # 带宽算法 (60 LOC)
│
├── platform/
│  ├── linux/
│  │  ├── taprio_if.h           # Linux tc/taprio 接口
│  │  └── taprio_if.c           # 接口实现
│  │
│  └── switch/
│     ├── switch_hal.h          # 硬件交换机 HAL
│     └── switch_hal.c          # HAL 实现
│
└── api/
   ├── tsn_api.h                # 公共 API
   └── tsn_api.c                # API 实现 (100 LOC)
```

### API 接口

```c
// api/tsn_api.h

/**
 * TSN 初始化
 */
int tsn_init(const tsn_config_t *config);

/**
 * 添加新流
 */
int tsn_add_flow(const tsn_flow_identifier_t *identifier,
                 const tsn_flow_properties_t *properties);

/**
 * 移除流
 */
int tsn_remove_flow(uint32_t flow_id);

/**
 * 查询流统计
 */
int tsn_get_flow_stats(uint32_t flow_id, tsn_flow_stats_t *stats);

/**
 * 更新门控配置
 */
int tsn_update_gate_config(const tsn_cycle_config_t *config);

/**
 * 获取当前统计
 */
int tsn_get_scheduler_stats(tsn_scheduler_stats_t *stats);

/**
 * 清理资源
 */
void tsn_deinit(void);
```

---

## Linux 集成

### tc/taprio 命令集成

```bash
# 1. 启用 taprio qdisc
sudo tc qdisc add dev eth0 root handle 100: taprio \
    num_tc 4 \
    map 0 0 1 1 2 2 3 3 \
    queues 1@0 1@1 1@2 1@3 \
    base-time 0 \
    sched-entry S 0x1 100 \
    sched-entry S 0x2 300 \
    sched-entry S 0x4 500 \
    sched-entry S 0x8 100 \
    clockid CLOCK_TAI

# 2. 添加 tc filter 分类流量
sudo tc filter add dev eth0 parent 100: protocol ip prio 1 u32 \
    match ip dport 5000 0xffff flowid 1:1

# 3. 查询当前配置
tc qdisc show dev eth0
tc class show dev eth0
tc filter show dev eth0
```

### Python API 包装

```python
#!/usr/bin/env python3
"""
TSN 调度器 Python API 包装
"""

import ctypes
import time

# 加载 C 库
libtsn = ctypes.CDLL('./libtsn.so')

class TSNScheduler:
    def __init__(self):
        self.handle = None
    
    def init(self, config):
        """初始化 TSN 调度器"""
        result = libtsn.tsn_init(config)
        return result == 0
    
    def add_flow(self, flow_id, src_ip, dst_ip, period_us, priority):
        """添加新流"""
        result = libtsn.tsn_add_flow(flow_id, src_ip, dst_ip, period_us, priority)
        return result == 0
    
    def get_stats(self):
        """获取统计信息"""
        stats = ctypes.c_void_p()
        libtsn.tsn_get_scheduler_stats(ctypes.byref(stats))
        return stats
    
    def deinit(self):
        """清理资源"""
        libtsn.tsn_deinit()

# 使用示例
if __name__ == '__main__':
    scheduler = TSNScheduler()
    scheduler.init(None)
    
    # 添加 RT 流
    scheduler.add_flow(
        flow_id=1,
        src_ip='192.168.1.100',
        dst_ip='192.168.1.200',
        period_us=1000,  # 1ms
        priority=7       # 最高优先级
    )
    
    time.sleep(5)
    scheduler.deinit()
```

---

## W2-W4 开发计划

### W2 (Jan 27 - Feb 2): 算法框架与队列管理

**目标**: 完成调度器框架代码 (200+ LOC)

| 任务 | 预期 | 交付物 |
|------|------|--------|
| 队列管理 | W2-1 | queue.h/c (120 LOC) |
| 门控算法 | W2-2 | scheduler.c (150 LOC) |
| 单元测试 | W2-3 | test_scheduler.c (>40% 覆盖) |
| Linux 适配 | W2-4 | taprio_if.h (初版) |

**交付**:
- [ ] `src/core/scheduler.c` - 核心调度逻辑 (150+ LOC)
- [ ] `src/core/queue.h/c` - 优先级队列实现
- [ ] `test/test_scheduler_unit.c` - 单元测试 (>40%)
- [ ] `SCHEDULER_PROGRESS.md` - W2 进度报告

### W3 (Feb 3-9): 流表与带宽控制

**目标**: 完整的流管理与入场控制

| 任务 | 预期 | 交付物 |
|------|------|--------|
| 流表管理 | W3-1 | flow.h/c (100 LOC) |
| 带宽控制 | W3-2 | bandwidth.c (80 LOC) |
| 入场算法 | W3-3 | admission.c (60 LOC) |
| 集成测试 | W3-4 | test_integration.c |

**交付**:
- [ ] 流表管理 & 查询接口
- [ ] 带宽预留与检查
- [ ] 入场控制算法
- [ ] 与 Linux tc/taprio 集成测试

### W4 (Feb 10-16): Linux 集成与优化

**目标**: 完整的 Linux 适配与性能优化

| 任务 | 预期 | 交付物 |
|------|------|--------|
| Linux taprio | W4-1 | taprio_if.c (100 LOC) |
| Python API | W4-2 | tsn_api.py (150 LOC) |
| 性能测试 | W4-3 | 性能基准测试报告 |
| 文档完善 | W4-4 | 使用手册 & API 文档 |

**交付**:
- [ ] 与 Linux tc/taprio 的无缝集成
- [ ] Python 控制 API
- [ ] 性能数据 (<500µs 延迟)
- [ ] 完整的使用文档

---

**制定日期**: 2026-01-20  
**当前阶段**: Stage 2 W2  
**目标完成**: 2026-02-16  
**设计评审**: 2026-01-27 (14:00)
