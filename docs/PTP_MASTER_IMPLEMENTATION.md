# PTP Master 消息处理实现设计

**版本**: v0.1.0  
**日期**: 2026-01-20  
**阶段**: Stage 2 Week 2  
**目标**: IEEE 1588 v2 Master 模式核心功能实现

---

## 📋 目录

1. [核心功能分解](#核心功能分解)
2. [Master 状态机详设](#master-状态机详设)
3. [消息处理流程](#消息处理流程)
4. [关键算法实现](#关键算法实现)
5. [代码结构与模块划分](#代码结构与模块划分)
6. [API 接口设计](#api-接口设计)
7. [测试策略](#测试策略)
8. [W2-W4 开发计划](#w2-w4-开发计划)

---

## 核心功能分解

### 第一阶段 (W2-W3): Master 基础框架

```
PTP Master 核心功能:
├─ ANNOUNCE 消息生成与广播
│  ├─ 构建 ANNOUNCE 报文
│  ├─ 定时发送 (默认 1s 周期)
│  └─ 时钟层级管理
│
├─ SYNC 消息生成与时间同步
│  ├─ 生成 SYNC 报文 + 时间戳
│  ├─ 发送 FOLLOW_UP (含精确 Tx 时间)
│  └─ 处理 DELAY_REQ/DELAY_RESP 对
│
├─ 时间维护
│  ├─ 维护本地时间 (seconds + nanoseconds)
│  ├─ Grandmaster 时间源
│  └─ 与系统时钟同步
│
└─ 状态管理
   ├─ M1(INITIALIZING) → M2(LISTENING) → M3(MASTER)
   ├─ 状态转移条件检查
   └─ 故障恢复 (clock jump 检测)
```

### 性能目标 (可测)

| 指标 | W2 目标 | W3 目标 | W4 目标 |
|------|--------|--------|--------|
| 消息构造延迟 | <100µs | <50µs | <20µs |
| ANNOUNCE 周期抖动 | ±10ms | ±5ms | ±1ms |
| SYNC 消息精度 | ±1ms | ±100µs | ±10µs |
| 内存占用 | <10KB | <8KB | <5KB |

---

## Master 状态机详设

### 状态定义

```c
/**
 * PTP Master 工作模式
 */
enum ptp_master_state {
    PTP_M_INITIALIZING = 1,  // 初始化状态
    PTP_M_LISTENING = 2,     // 监听模式(竞争 Grandmaster)
    PTP_M_MASTER = 3,        // Master 角色
    PTP_M_DISABLED = 4       // 禁用
};
```

### 完整状态转移图

```
┌────────────────────────────────────────────────────────┐
│                    INITIALIZING                        │
│  - 初始化时钟                                         │
│  - 加载配置参数                                       │
│  - 启动网络接口                                       │
└────────────────┬─────────────────────────────────────┘
                 │ 初始化成功
                 ▼
┌────────────────────────────────────────────────────────┐
│                     LISTENING                          │
│  - 监听 ANNOUNCE 消息                                 │
│  - 比较 Grandmaster ID                                │
│  - 计算 Best Master Clock (BMC) 算法                  │
└────────────────┬─────────────────────────────────────┘
                 │
    ┌────────────┴─────────────┐
    │ 本设备是 Best Master?    │
    └────────────┬─────────────┘
                 │
        ┌────────┴────────┐
    YES │                 │ NO (收到更优 Master)
        ▼                 ▼
      MASTER           继续 LISTENING
        │
    ├─ 发送 ANNOUNCE
    ├─ 发送 SYNC + FOLLOW_UP
    ├─ 处理 DELAY_REQ/RESP
    └─ 维护时钟

LISTENING <─┐ (clock jump 触发重新 BMC)
            │
      [故障恢复]
```

### 状态转移条件

```c
/**
 * Master 状态转移条件检查
 */
struct ptp_master_transition {
    // INITIALIZING → LISTENING
    bool initialization_success;      // 初始化无误
    bool network_available;           // 网络就绪
    bool clock_valid;                 // 时钟有效
    
    // LISTENING → MASTER
    bool is_best_master;              // BMC 计算为最优
    bool announce_interval_ok;        // ANNOUNCE 间隔正确
    bool no_superior_master;          // 无更优的 Master
    
    // MASTER → LISTENING (故障)
    bool clock_jump_detected;         // 时钟跳变 >1s
    bool network_lost;                // 网络断开
    bool power_loss;                  // 掉电
};
```

---

## 消息处理流程

### ANNOUNCE 消息生成 (1Hz)

```c
/**
 * 生成 ANNOUNCE 消息
 * 周期: 1Hz (可配置)
 * 大小: 64 bytes
 */
typedef struct {
    // PTP 报文头 (34 bytes)
    uint16_t transport;           // UDP
    uint8_t version;              // 0x02
    uint16_t message_len;         // 64
    uint8_t domain;               // 默认 0
    uint8_t flags;                // 0x00
    int64_t correction_field;     // 0
    uint8_t source_port_id[10];   // Port ID
    uint16_t seq_id;              // 自增
    uint8_t ctrl_field;           // 0x00 (Others)
    int8_t log_interval;          // log2(1) = 0
    
    // ANNOUNCE 负载 (30 bytes)
    int64_t origin_timestamp;     // 来源时间
    uint16_t utc_offset;          // UTC 偏移
    uint8_t grandmaster_priority1;// 优先级 1
    uint8_t grandmaster_class;    // 时钟类
    uint16_t grandmaster_accuracy;// 精度指示
    uint32_t grandmaster_variance;// 时钟方差
    uint8_t grandmaster_priority2;// 优先级 2
    uint8_t grandmaster_id[8];    // Grandmaster ID
    uint16_t steps_removed;       // 0 (Master)
    uint8_t time_source;          // GPS/PTP/etc
    
} ptp_announce_message_t;

// 伪代码
void ptp_master_send_announce(void) {
    ptp_announce_message_t announce;
    
    // 填充报文头
    announce.version = 0x02;
    announce.message_len = 64;
    announce.seq_id++;
    announce.log_interval = 0;  // 1Hz
    
    // 填充 ANNOUNCE 负载
    announce.origin_timestamp = ptp_get_time();
    announce.grandmaster_priority1 = 128;  // 默认优先级
    announce.steps_removed = 0;
    
    // 广播到所有端口
    for (int port = 0; port < PTP_NUM_PORTS; port++) {
        ptp_send_packet(port, &announce, 64);
    }
}
```

### SYNC 消息生成 (实时同步)

```c
/**
 * 生成 SYNC 消息 (Master 发送)
 * 大小: 44 bytes
 */
typedef struct {
    // 报文头 (34 bytes)
    uint16_t transport;
    uint8_t version;              // 0x02
    uint16_t message_len;         // 44
    uint8_t domain;
    uint8_t flags;
    int64_t correction_field;
    uint8_t source_port_id[10];
    uint16_t seq_id;              // 自增
    uint8_t ctrl_field;           // 0x00 (Sync)
    int8_t log_interval;          // log2(16) = 4 (16s周期)
    
    // SYNC 负载 (10 bytes)
    uint64_t origin_timestamp;    // 发送时间戳
    
} ptp_sync_message_t;

/**
 * 在中断中采集精确 Tx 时间戳
 * 在后续 FOLLOW_UP 中发送
 */
void ptp_master_send_sync(void) {
    ptp_sync_message_t sync;
    uint64_t tx_timestamp;
    
    // 填充报文头
    sync.version = 0x02;
    sync.message_len = 44;
    sync.seq_id++;
    sync.log_interval = 4;  // 16s周期 (可配置)
    
    // 设置 correction_field 为 0 (Master不做更正)
    sync.correction_field = 0;
    
    // 采集发送前时间 (近似)
    uint64_t tx_approx = ptp_get_time();
    sync.origin_timestamp = tx_approx;
    
    // 发送
    for (int port = 0; port < PTP_NUM_PORTS; port++) {
        tx_timestamp = ptp_send_packet_with_timestamp(port, &sync, 44);
        
        // 存储精确 tx_timestamp 用于 FOLLOW_UP
        ptp_pending_tx_timestamps[seq_id] = tx_timestamp;
    }
}

/**
 * FOLLOW_UP 消息 (紧跟 SYNC 之后)
 * 大小: 44 bytes
 */
typedef struct {
    uint16_t transport;
    uint8_t version;              // 0x02
    uint16_t message_len;         // 44
    uint8_t domain;
    uint8_t flags;
    int64_t correction_field;     // 0
    uint8_t source_port_id[10];
    uint16_t seq_id;              // 与 SYNC 相同
    uint8_t ctrl_field;           // 0x02 (Follow_Up)
    int8_t log_interval;          // 与 SYNC 相同
    
    // FOLLOW_UP 负载 (10 bytes)
    uint64_t precise_origin_timestamp;  // 精确 Tx 时间戳
    
} ptp_follow_up_message_t;

void ptp_master_send_follow_up(uint16_t sync_seq_id) {
    ptp_follow_up_message_t follow_up;
    
    // 查找对应的 SYNC Tx 时间戳
    uint64_t precise_tx = ptp_pending_tx_timestamps[sync_seq_id];
    
    follow_up.seq_id = sync_seq_id;  // 与 SYNC 配对
    follow_up.precise_origin_timestamp = precise_tx;
    
    // 广播
    for (int port = 0; port < PTP_NUM_PORTS; port++) {
        ptp_send_packet(port, &follow_up, 44);
    }
}
```

### DELAY_REQ 处理

```c
/**
 * Master 接收 DELAY_REQ 并发送 DELAY_RESP
 * 
 * Slave 发送 DELAY_REQ
 *   ↓
 * Master 接收 + 采集 Rx 时间戳
 *   ↓
 * Master 发送 DELAY_RESP (含 Rx 时间戳)
 */

void ptp_master_process_delay_req(const uint8_t *packet, uint16_t len, 
                                   uint64_t rx_timestamp) {
    ptp_delay_req_message_t *req = (ptp_delay_req_message_t *)packet;
    ptp_delay_resp_message_t resp;
    
    // 构建 DELAY_RESP
    resp.version = 0x02;
    resp.message_len = 54;
    resp.seq_id = req->seq_id;  // 配对
    resp.ctrl_field = 0x03;     // DELAY_RESP
    
    // 存储接收时间戳
    resp.receive_timestamp = rx_timestamp;
    
    // 发送 DELAY_RESP
    ptp_send_packet(0, &resp, 54);
}
```

---

## 关键算法实现

### Best Master Clock (BMC) 算法

```c
/**
 * BMC 算法: 决定本设备是否应为 Master
 * 比较规则 (优先级从高到低):
 * 1. Grandmaster Priority1 (越低越优)
 * 2. Grandmaster Class (越小越优)
 * 3. Grandmaster Accuracy (越小越优)
 * 4. Grandmaster Variance (越小越优)
 * 5. Grandmaster Priority2 (越低越优)
 * 6. Grandmaster ID (字典序越小越优)
 */

typedef struct {
    uint8_t grandmaster_priority1;
    uint8_t grandmaster_class;
    uint16_t grandmaster_accuracy;
    uint32_t grandmaster_variance;
    uint8_t grandmaster_priority2;
    uint8_t grandmaster_id[8];
    uint16_t steps_removed;
} ptp_grandmaster_data_t;

/**
 * 比较两个 Grandmaster
 * 返回: -1 (master1 更优), 0 (相等), 1 (master2 更优)
 */
int ptp_bmc_compare(const ptp_grandmaster_data_t *gm1,
                    const ptp_grandmaster_data_t *gm2) {
    // 1. 比较 Priority1
    if (gm1->grandmaster_priority1 != gm2->grandmaster_priority1) {
        return (gm1->grandmaster_priority1 < gm2->grandmaster_priority1) ? -1 : 1;
    }
    
    // 2. 比较 Class
    if (gm1->grandmaster_class != gm2->grandmaster_class) {
        return (gm1->grandmaster_class < gm2->grandmaster_class) ? -1 : 1;
    }
    
    // 3. 比较 Accuracy
    if (gm1->grandmaster_accuracy != gm2->grandmaster_accuracy) {
        return (gm1->grandmaster_accuracy < gm2->grandmaster_accuracy) ? -1 : 1;
    }
    
    // 4. 比较 Variance
    if (gm1->grandmaster_variance != gm2->grandmaster_variance) {
        return (gm1->grandmaster_variance < gm2->grandmaster_variance) ? -1 : 1;
    }
    
    // 5. 比较 Priority2
    if (gm1->grandmaster_priority2 != gm2->grandmaster_priority2) {
        return (gm1->grandmaster_priority2 < gm2->grandmaster_priority2) ? -1 : 1;
    }
    
    // 6. 比较 ID (字典序)
    return memcmp(gm1->grandmaster_id, gm2->grandmaster_id, 8);
}

/**
 * 更新最优 Grandmaster
 */
void ptp_master_update_best_master(const ptp_grandmaster_data_t *candidate) {
    static ptp_grandmaster_data_t best_master = {0};
    
    if (ptp_bmc_compare(candidate, &best_master) < 0) {
        // candidate 更优
        best_master = *candidate;
        
        if (best_master.steps_removed == 0) {
            // 本地是最优 Master
            ptp_state = PTP_M_MASTER;
        } else {
            ptp_state = PTP_M_LISTENING;
        }
    }
}
```

### 时钟维护

```c
/**
 * 全局时钟维护
 * 类似 POSIX time: uint64_t seconds + uint32_t nanoseconds
 */

typedef struct {
    uint64_t seconds;        // Unix 时间戳
    uint32_t nanoseconds;    // 纳秒部分
} ptp_time_t;

volatile ptp_time_t g_ptp_time = {0};
volatile uint64_t g_ptp_cycles = 0;  // 系统 tick 计数

/**
 * 中断处理: 每秒更新一次秒数
 */
void ptp_timer_1s_irq(void) {
    g_ptp_time.seconds++;
    g_ptp_time.nanoseconds = 0;
    
    // 每秒发送一次 ANNOUNCE
    ptp_master_send_announce();
}

/**
 * 高频获取当前时间
 * 使用硬件计数器精确度量纳秒
 */
ptp_time_t ptp_get_time(void) {
    ptp_time_t result;
    uint64_t hw_counter = ptp_hw_get_nanoseconds();  // 硬件计数器
    
    // 无中断访问全局时间
    uint64_t sec = g_ptp_time.seconds;
    uint32_t nsec = g_ptp_time.nanoseconds;
    
    // 累加硬件计数
    nsec += (hw_counter % 1000000000);
    if (nsec >= 1000000000) {
        nsec -= 1000000000;
        sec++;
    }
    
    result.seconds = sec;
    result.nanoseconds = nsec;
    return result;
}

/**
 * 同步系统时间 (仅 Master 调用)
 * 如果本地时钟与外部时钟差异过大，触发故障恢复
 */
bool ptp_master_check_clock_jump(ptp_time_t external_time) {
    ptp_time_t local_time = ptp_get_time();
    int64_t diff_ns = (int64_t)(external_time.seconds - local_time.seconds) * 1000000000
                    + (int64_t)(external_time.nanoseconds - local_time.nanoseconds);
    
    if (abs(diff_ns) > 1000000000) {  // >1s
        // 时钟跳变，触发重新选举
        PTP_LOG_WARN("Clock jump detected: %lld ns", diff_ns);
        return true;  // 需要重新进入 LISTENING
    }
    return false;
}
```

---

## 代码结构与模块划分

### 文件组织

```
ptp-stack/src/master/
├── master.h                    # Master 头文件
├── master.c                    # Master 核心实现
├── messages.h                  # PTP 消息定义
├── messages.c                  # 消息构造与解析
├── bmc.h                       # BMC 算法
├── bmc.c                       # BMC 实现
├── clock.h                     # 时钟管理
└── clock.c                     # 时钟实现
```

### 核心 API 模块

```c
// master.h

/**
 * Master 初始化
 */
int ptp_master_init(const ptp_master_config_t *config);

/**
 * 主循环处理 (每 10ms 调用一次)
 */
int ptp_master_step(void);

/**
 * 处理接收的数据包
 */
int ptp_master_process_packet(const uint8_t *packet, uint16_t len, 
                              uint64_t rx_timestamp);

/**
 * 获取当前同步状态
 */
ptp_sync_status_t ptp_master_get_sync_status(void);

/**
 * 清理资源
 */
void ptp_master_deinit(void);
```

---

## API 接口设计

### 配置结构体

```c
typedef struct {
    uint8_t domain;
    uint8_t priority1;
    uint8_t priority2;
    uint8_t clock_class;
    uint16_t clock_accuracy;
    uint32_t clock_variance;
    uint8_t time_source;
    
    // 消息间隔 (log2 格式)
    int8_t announce_interval;     // 默认 0 (1Hz)
    int8_t sync_interval;         // 默认 4 (16s)
    int8_t delay_resp_interval;   // 默认 4 (16s)
    
    // 网络配置
    uint16_t udp_port;            // 默认 319
    uint8_t multicast_addr[4];    // 224.0.1.129
    
} ptp_master_config_t;
```

### 状态查询

```c
typedef struct {
    uint8_t state;               // MASTER / LISTENING
    uint64_t announce_sent;      // 已发送 ANNOUNCE 数
    uint64_t sync_sent;          // 已发送 SYNC 数
    uint64_t delay_resp_sent;    // 已发送 DELAY_RESP 数
    
    uint32_t packets_sent;       // 总发送数
    uint32_t packets_received;   // 总接收数
    uint32_t errors;             // 错误数
    
    ptp_time_t current_time;     // 当前时间
    uint16_t steps_removed;      // 步数 (Master 为 0)
    
} ptp_sync_status_t;
```

---

## 测试策略

### 单元测试 (目标: 50%+ 覆盖率)

```c
// test/test_master_messages.c

void test_announce_construction(void) {
    // 测试 ANNOUNCE 消息构造
    ptp_announce_message_t announce;
    ptp_master_build_announce(&announce);
    
    assert_equal(announce.version, 0x02);
    assert_equal(announce.message_len, 64);
    assert_equal(announce.steps_removed, 0);
}

void test_bmc_algorithm(void) {
    // 测试 BMC 比较
    ptp_grandmaster_data_t gm1 = {128, 6, 0, 0, 128, {0}, 0};
    ptp_grandmaster_data_t gm2 = {128, 6, 0, 0, 128, {1}, 0};
    
    int result = ptp_bmc_compare(&gm1, &gm2);
    assert_equal(result, -1);  // gm1 优于 gm2
}

void test_clock_jump_detection(void) {
    // 测试时钟跳变检测
    ptp_time_t external = {1000, 500000000};
    bool jumped = ptp_master_check_clock_jump(external);
    
    // 当前时间是 0，相差 1000s，应该检测到跳变
    assert_true(jumped);
}
```

### 集成测试 (W3-W4)

- [ ] PTP Master 与 Slave 同步测试
- [ ] 多 Master 竞争测试
- [ ] 时钟精度测试 (<100µs)
- [ ] 网络延迟处理测试
- [ ] 故障恢复测试

### 性能测试

```
性能指标追踪:
├─ 消息构造延迟 (target <100µs)
├─ ANNOUNCE 周期稳定性 (target ±10ms)
├─ 内存占用 (target <10KB)
└─ CPU 使用率 (target <5%)
```

---

## W2-W4 开发计划

### W2 (Jan 27 - Feb 2): Master 框架与消息处理

**目标**: 完成 Master 核心代码框架 (200+ LOC)

| 任务 | 预期 | 交付物 |
|------|------|--------|
| 消息定义 & 构造 | W2-1 | messages.h/c (150 LOC) |
| BMC 算法实现 | W2-2 | bmc.h/c (100 LOC) |
| 时钟管理 | W2-3 | clock.h/c (80 LOC) |
| 单元测试 | W2-4 | test_master.c (>50% 覆盖) |

**交付:
- [ ] `src/master/master.h` - Master 头文件
- [ ] `src/master/master.c` - 核心代码框架 (200+ LOC)
- [ ] `test/test_master_unit.c` - 单元测试
- [ ] `docs/MASTER_IMPLEMENTATION_PROGRESS.md` - 周进度报告

### W3 (Feb 3-9): Master 消息处理完善

**目标**: 完整的 SYNC/FOLLOW_UP/DELAY_RESP 流程

| 任务 | 预期 | 交付物 |
|------|------|--------|
| SYNC 流程 | W3-1 | sync.c (100 LOC) |
| DELAY_REQ/RESP | W3-2 | delay.c (80 LOC) |
| 定时管理 | W3-3 | timing.c (60 LOC) |
| 集成测试 | W3-4 | test_integration.c |

**交付**:
- [ ] 完整 SYNC/FOLLOW_UP 处理流程
- [ ] DELAY_REQ/RESP 完整对
- [ ] 定时发送机制 (1Hz ANNOUNCE, configurable SYNC)
- [ ] 集成测试报告

### W4 (Feb 10-16): 平台适配与优化

**目标**: 支持 3 平台 (STM32H7, RK3588, ESP32)

| 任务 | 预期 | 交付物 |
|------|------|--------|
| STM32H7 适配 | W4-1 | platform/stm32h7/master_hal.c |
| RK3588 适配 | W4-2 | platform/rk3588/master_hal.c |
| ESP32 适配 | W4-3 | platform/esp32/master_hal.c |
| 性能优化 | W4-4 | 优化报告 + 性能数据 |

**交付**:
- [ ] 3 平台 Master HAL 实现
- [ ] 性能基准测试报告
- [ ] 准备 Demo 与集成测试

---

**制定日期**: 2026-01-20  
**当前阶段**: Stage 2 W2  
**目标完成**: 2026-02-16  
**下一审查**: 2026-02-03
