# PTP 消息处理 - 深度讲解与实现指南

> 📚 **适用范围**: M2.1 Alpha W2-W3 的 PTP 消息实现  
> **目标**: 从协议理解到代码实现的完整链路  
> **难度**: 中等 | **时间**: 4-6 小时掌握

---

## 第一部分：核心概念

### 1.1 为什么需要 PTP?

在分布式机器人系统中，多个控制器需要**精确同步**：

```
例子: 四足机器人四条腿同时动作
  └─ 如果时间不同步，腿部动作会有延迟
  └─ 结果: 机器人跌倒!

精度要求:
  • DDS: ±10ms (太差, 能看到抖动)
  • ROS 2 标准: ±1ms (勉强)
  • PTP (我们的目标): ±10µs (完全看不出延迟)
```

### 1.2 PTP 的关键优势

| 方案 | 精度 | 成本 | 复杂度 | 开源度 |
|------|------|------|--------|--------|
| GPS | ±100ns | ¥500+ | 高 | 开源 |
| **PTP** | **±10µs** | **¥5** | **中** | **开源** |
| 原子钟 | ±1ns | ¥∞ | 极高 | 专利 |
| 网络同步 | ±100ms | ¥0 | 低 | 开源 |

**PTP 的独特优势**: 硬件成本低 + 精度高 + 易于集成

---

## 第二部分：核心消息详解

### 2.1 ANNOUNCE 消息 - Master 宣告

#### 用途
- Master 定期向所有设备广播："我是时钟源"
- 其他设备通过 BMC 算法比较，选出最优 Master

#### 报文结构

```
ANNOUNCE 消息 (64 bytes)
├─ 报文头 (34 bytes)
│  ├─ transport_specific (4 bits): PTP_OVER_UDP_IPV4
│  ├─ message_type (4 bits): ANNOUNCE (11)
│  ├─ version (4 bits): 2
│  ├─ message_length (16 bits): 64
│  ├─ domain (8 bits): 0 (默认域)
│  ├─ source_port_id (80 bits): 本设备 MAC + 端口号
│  ├─ sequence_id (16 bits): 自增计数器 ← 关键!
│  └─ log_message_interval (8 bits): -3 (2^-3 = 125ms)
│
└─ 负载 (30 bytes)
   ├─ origin_timestamp (64 bits): 消息源时间
   ├─ grandmaster_priority1 (8 bits): 优先级 1 ← 关键!
   ├─ grandmaster_class (8 bits): 时钟类型 (ATOM/GPS/etc)
   ├─ grandmaster_accuracy (16 bits): 精度指示 (纳秒)
   ├─ grandmaster_variance (32 bits): 时钟方差
   ├─ grandmaster_priority2 (8 bits): 优先级 2 ← 关键!
   ├─ grandmaster_id (64 bits): 唯一标识 (通常 MAC 地址)
   ├─ steps_removed (16 bits): 0 (Master), >0 (中间节点)
   └─ time_source (8 bits): 0x10 (INTERNAL_OSCILLATOR)
```

#### BMC 算法 - 如何选出最优 Master?

比较规则 (按优先级从高到低)：

```
比较顺序:
  1. priority1 (越低越优) → 分配给主 Master
  2. class (越小越优) → ATOM(0x10) < GPS(0x20)
  3. accuracy (越小越优) → ±100ns 优于 ±1µs
  4. variance (越小越优) → 时钟稳定性
  5. priority2 (越低越优) → 次级优先级
  6. id (字典序) → 作为最后的平局方式
```

代码示例：

```c
// 比较两个 Grandmaster
int compare_gm(const ptp_gm_t *gm1, const ptp_gm_t *gm2) {
    // 1. 比较 priority1
    if (gm1->priority1 != gm2->priority1) {
        return (gm1->priority1 < gm2->priority1) ? -1 : 1;
    }
    
    // 2. 比较 class (时钟类型)
    if (gm1->clock_class != gm2->clock_class) {
        return (gm1->clock_class < gm2->clock_class) ? -1 : 1;
    }
    
    // 3. 比较 accuracy (精度)
    uint16_t acc1 = gm1->accuracy;  // ±ns
    uint16_t acc2 = gm2->accuracy;
    if (acc1 != acc2) {
        return (acc1 < acc2) ? -1 : 1;
    }
    
    // 4. 比较 variance (稳定性)
    uint32_t var1 = gm1->variance;
    uint32_t var2 = gm2->variance;
    if (var1 != var2) {
        return (var1 < var2) ? -1 : 1;
    }
    
    // 5. 比较 priority2
    if (gm1->priority2 != gm2->priority2) {
        return (gm1->priority2 < gm2->priority2) ? -1 : 1;
    }
    
    // 6. 比较 ID (字典序)
    return memcmp(gm1->id, gm2->id, 8);
}

// 使用
int is_better = compare_gm(&candidate, &current_best);
if (is_better < 0) {
    // candidate 更优，更新 current_best
    current_best = candidate;
}
```

---

### 2.2 SYNC + FOLLOW_UP - 时间同步对

#### 为什么分成两个消息?

**问题：** 软件无法在发送时刻立即知道硬件时间戳

```
发送 SYNC 的流程:
  1. CPU: 构造 SYNC 报文
  2. CPU: 调用发送函数 send()
  3. DMA: 报文加载到网卡
  4. 网卡: 报文进入物理层 ← 这里网卡采集精确时间戳!
  5. CPU: 返回 (此时还不知道 Tx 时间)
  6. 中断: 网卡报告硬件时间戳 (延迟 0-100µs)
  7. CPU: 在中断中知道了精确 Tx 时间!
```

**解决方案：** 分离消息

```
方案 1 (错误 - 精度差):
  SYNC [t1 ≈ 200ms (估算)]
  └─ 误差: ±1ms

方案 2 (正确 - 精度好):
  SYNC [t1 ≈ 200ms (估算)]     // 立即发送
  HW采集                    ← 网卡在中断中采集精确时间
  FOLLOW_UP [t1 = 200.000123ms] // 随后发送精确值
  └─ 误差: ±10µs (精度提升 100 倍!)
```

#### 报文结构

**SYNC 消息 (44 bytes):**

```c
typedef struct {
    // 报文头 (34 bytes)
    uint8_t  message_type;        // 0x00 (SYNC)
    uint8_t  version;             // 0x02
    uint16_t message_length;      // 44
    uint8_t  domain;              // 0
    int64_t  correction_field;    // 0 (Master 不需要)
    uint8_t  source_port_id[10];  // 本 Port 的 ID
    uint16_t sequence_id;         // 自增序列号 ← 关键!
    uint8_t  control;             // 0x00 (SYNC)
    int8_t   log_interval;        // 4 (16s 周期)
    
    // 负载 (10 bytes)
    uint64_t origin_timestamp;    // Master 发送时间 (ns)
    
} ptp_sync_message_t;
```

**FOLLOW_UP 消息 (44 bytes):**

```c
typedef struct {
    // 报文头 (34 bytes)
    uint8_t  message_type;        // 0x08 (FOLLOW_UP)
    uint8_t  version;             // 0x02
    uint16_t message_length;      // 44
    uint8_t  domain;              // 0
    int64_t  correction_field;    // 0
    uint8_t  source_port_id[10];  // 与 SYNC 相同
    uint16_t sequence_id;         // 与 SYNC 相同 ← 配对的关键!
    uint8_t  control;             // 0x02 (FOLLOW_UP)
    int8_t   log_interval;        // 与 SYNC 相同
    
    // 负载 (10 bytes)
    uint64_t precise_origin_timestamp;  // 精确发送时间
    
} ptp_follow_up_message_t;
```

#### 发送实现

```c
// 全局状态
struct {
    uint16_t sync_seq_id;
    uint64_t sync_tx_timestamp;
} ptp_state;

void ptp_master_send_sync(void) {
    ptp_sync_message_t sync;
    
    // 填充报文头
    sync.message_type = 0x00;
    sync.version = 0x02;
    sync.message_length = 44;
    sync.sequence_id = ptp_state.sync_seq_id++;
    sync.log_interval = 4;  // 16s 周期 (2^4 = 16s)
    sync.correction_field = 0;
    
    // 填充负载 - 发送时的近似时间
    sync.origin_timestamp = ptp_get_time();  // 估算时间
    
    // 发送
    for (int port = 0; port < NUM_PORTS; port++) {
        ptp_send_to_port(port, (uint8_t *)&sync, 44);
    }
    
    // 存储 seq_id，待硬件时间戳采集后用于发送 FOLLOW_UP
    ptp_state.sync_seq_id = sync.sequence_id;
}

// 在中断中被调用（网卡报告硬件时间戳）
void ptp_on_tx_timestamp(uint16_t seq_id, uint64_t hw_timestamp) {
    if (seq_id != ptp_state.sync_seq_id) {
        return;  // 不是我们的消息
    }
    
    // 构造 FOLLOW_UP
    ptp_follow_up_message_t follow_up;
    follow_up.message_type = 0x08;
    follow_up.version = 0x02;
    follow_up.sequence_id = seq_id;  // 配对!
    follow_up.precise_origin_timestamp = hw_timestamp;  // 精确时间
    
    // 广播 FOLLOW_UP
    for (int port = 0; port < NUM_PORTS; port++) {
        ptp_send_to_port(port, (uint8_t *)&follow_up, 44);
    }
}
```

---

### 2.3 DELAY_REQ + DELAY_RESP - 网络延迟测量

#### 原理

Slave 需要知道单向网络延迟，以计算精确的时间误差。

```
消息交换:
  Slave                 Master
    │                     │
    │ ① DELAY_REQ (t3)    │
    ├──────────────────────▶ ② 接收 (t4)
    │                     │
    │ ③ DELAY_RESP (t4)  │
    │◀────────────────────┤
    ④ 接收 (t5)          │
    │                     │

计算:
  往返延迟 = (t5 - t3) - (t4 - t4)  ← 不对!
  
  正确计算:
  往返延迟 = (t5 - t3)  // 总耗时
  单向延迟 ≈ 往返延迟 / 2  // 假设对称
```

#### 报文结构

```c
typedef struct {
    // 报文头 (34 bytes)
    uint8_t  message_type;        // 0x13 (DELAY_REQ)
    uint8_t  version;             // 0x02
    uint16_t message_length;      // 44
    uint8_t  domain;              // 0
    int64_t  correction_field;    // 路径延迟
    uint8_t  source_port_id[10];  // Slave 的 Port ID
    uint16_t sequence_id;         // 自增序列号
    uint8_t  control;             // 0x01 (DELAY_REQ)
    int8_t   log_interval;        // -4 (2^-4 = 62.5ms)
    
    // 负载 (10 bytes)
    uint64_t origin_timestamp;    // Slave 发送时间
    
} ptp_delay_req_message_t;

typedef struct {
    // 报文头 (34 bytes)
    uint8_t  message_type;        // 0x03 (DELAY_RESP)
    uint8_t  version;             // 0x02
    uint16_t message_length;      // 54 (比 DELAY_REQ 多 10 bytes)
    uint8_t  domain;              // 0
    int64_t  correction_field;    // 路径延迟
    uint8_t  source_port_id[10];  // Master 的 Port ID
    uint16_t sequence_id;         // 与 DELAY_REQ 相同
    uint8_t  control;             // 0x03 (DELAY_RESP)
    int8_t   log_interval;        // 与 DELAY_REQ 相同
    
    // 负载 (20 bytes)
    uint64_t receive_timestamp;   // Master 接收时间 ← 关键!
    uint8_t  requesting_port_id[10];  // Slave 的 Port ID
    
} ptp_delay_resp_message_t;
```

#### 实现

```c
// Slave 发送 DELAY_REQ
void ptp_slave_send_delay_req(void) {
    ptp_delay_req_message_t req;
    
    req.message_type = 0x13;
    req.version = 0x02;
    req.sequence_id = ptp_state.delay_seq_id++;
    req.origin_timestamp = ptp_get_time();  // 发送时间
    
    ptp_send_to_master(&req, 44);
    
    // 保存发送时间
    ptp_state.delay_req_tx_time = req.origin_timestamp;
}

// Master 接收 DELAY_REQ 并发送 DELAY_RESP
void ptp_master_on_delay_req(const uint8_t *packet, uint64_t rx_timestamp) {
    ptp_delay_req_message_t *req = (ptp_delay_req_message_t *)packet;
    ptp_delay_resp_message_t resp;
    
    resp.message_type = 0x03;
    resp.version = 0x02;
    resp.sequence_id = req->sequence_id;  // 配对!
    resp.receive_timestamp = rx_timestamp;  // 接收时间戳
    memcpy(resp.requesting_port_id, req->source_port_id, 10);
    
    ptp_send_unicast_to_slave(&resp, 54);
}

// Slave 接收 DELAY_RESP 并计算延迟
void ptp_slave_on_delay_resp(const uint8_t *packet) {
    ptp_delay_resp_message_t *resp = (ptp_delay_resp_message_t *)packet;
    uint64_t now = ptp_get_time();  // 接收时间
    
    // 计算往返延迟
    uint64_t delay_req_tx = ptp_state.delay_req_tx_time;
    uint64_t delay_req_rx = resp->receive_timestamp;
    uint64_t delay_resp_rx = now;
    
    // 往返延迟 = 现在 - 发送时间
    int64_t round_trip_delay = (delay_resp_rx - delay_req_tx);
    
    // 假设网络对称，单向延迟
    int64_t one_way_delay = round_trip_delay / 2;
    
    ptp_state.mean_path_delay = one_way_delay;
}
```

---

## 第三部分：时间同步算法

### 3.1 时间误差计算

Slave 收到 SYNC 和 DELAY_RESP 后，计算当前时间误差：

```
已知数据:
  t1 = SYNC 中 Master 发送时间
  t2 = Slave 接收 SYNC 时间 (硬件时间戳)
  t4 = DELAY_RESP 中 Master 接收 DELAY_REQ 时间
  t5 = Slave 发送 DELAY_REQ 时间

时延计算:
  单向延迟 = ((t2 - t1) + (t4 - t5)) / 2
  
时间误差:
  offset = (t2 - t1) - 单向延迟
  
公式推导:
  offset = (t1 + 单向延迟) - t2  // Master 时间到 Slave 的转换
         = (t1 + ((t2-t1) + (t4-t5))/2) - t2
         = (t2 - t1 + t4 - t5) / 2  // 简化
```

代码：

```c
void ptp_slave_calc_offset(void) {
    // 收集所有时间戳
    uint64_t t1 = ptp_state.sync_tx_time;      // SYNC 中的发送时间
    uint64_t t2 = ptp_state.sync_rx_timestamp; // Slave 接收 SYNC
    uint64_t t4 = ptp_state.delay_rx_time;     // DELAY_REQ 被 Master 接收
    uint64_t t5 = ptp_state.delay_tx_time;     // Slave 发送 DELAY_REQ
    
    // 计算单向延迟
    int64_t delay = ((int64_t)(t2 - t1) + (int64_t)(t4 - t5)) / 2;
    
    // 计算时间误差 (纳秒级)
    int64_t offset = ((int64_t)(t2 - t1) + (int64_t)(t4 - t5)) / 2;
    offset -= ptp_state.mean_path_delay;
    
    ptp_state.time_offset = offset;
}
```

### 3.2 PI Servo - 频率调整

目标：**不跳变时钟**（会导致应用崩溃），而是缓慢调整频率。

```
原理:
  时钟误差: offset = 50µs (Slave 比 Master 快)
  
  方案 1: 直接跳变 ❌
    offset -= 50µs
    结果: 应用看到时间倒流, 大量代码崩溃
  
  方案 2: 调整频率 ✅
    本来: 1000MHz
    现在: 1000MHz - 50PPM (百万分之)
    效果: 每秒慢 50ns, 经过 1,000,000 秒 ≈ 11 天收敛
    问题: 太慢!
  
  方案 3: PI 调整 (最优) ✅✅
    P 增益: 快速大幅调整 (响应大误差)
    I 增益: 缓慢微调 (消除小误差)
    结果: 10 秒内收敛到 <1µs
```

#### PI 算法详解

```
PI 公式:
  freq_adjustment = KP * error + KI * integral_error
  
  KP = 0.7 (比例增益)
    - 当 error = 100µs 时: 0.7 * 100 = 70 PPM
    - 迅速反应大误差
  
  KI = 0.3 (积分增益)
    - 累积所有历史误差
    - 缓慢消除小的系统误差
    - 防止超调 (overshoot)
  
  CLAMP (限制器):
    - -500 ~ +500 PPM (±0.05% 频率偏差)
    - 防止过度调整
    - 保护硬件时钟
```

代码实现：

```c
struct {
    int64_t integral_sum;  // 累积误差
    int32_t last_freq_adj; // 上次频率调整
    
    #define KP_NUM 7  // 0.7
    #define KP_DEN 10
    #define KI_NUM 3  // 0.3
    #define KI_DEN 10
    #define MAX_PPM 500
    
} ptp_servo;

void ptp_servo_step(int64_t offset_ns) {
    // P 项: 快速响应
    int64_t p_term = (offset_ns * KP_NUM) / KP_DEN;
    
    // I 项: 缓慢消除
    ptp_servo.integral_sum += offset_ns;
    int64_t i_term = (ptp_servo.integral_sum * KI_NUM) / KI_DEN;
    
    // 计算频率调整 (PPM)
    int32_t freq_adj = (int32_t)((p_term + i_term) / 1000000);  // ns → PPM
    
    // 限制范围
    if (freq_adj > MAX_PPM) freq_adj = MAX_PPM;
    if (freq_adj < -MAX_PPM) freq_adj = -MAX_PPM;
    
    // 只在变化时调整 (减少系统抖动)
    if (freq_adj != ptp_servo.last_freq_adj) {
        platform_adjust_clock(freq_adj);  // 硬件调用
        ptp_servo.last_freq_adj = freq_adj;
    }
}

// 硬件平台特定实现 (以 STM32H7 为例)
void platform_adjust_clock(int32_t ppm) {
    // STM32H7 的主时钟由 PLL 提供 (400MHz)
    // 通过调整 PLL 分频因子改变频率
    // 1 PPM = 400 × 1000 / 1000000 = 0.4 Hz 调整
    
    uint32_t pll_m = 5;    // 输入预分频
    uint32_t pll_n = 160;  // PLL 乘数
    uint32_t pll_p = 2;    // 输出分频
    
    // 根据 ppm 调整 pll_n (简化版)
    uint32_t new_pll_n = pll_n + (ppm / 1000);
    
    // 更新 PLL (实际操作需要重新配置时钟)
    // ...
}
```

---

## 第四部分：完整工作流

### 4.1 一个完整同步周期

```
时间    Master                          Slave
─────────────────────────────────────────────────────
 0ms   ├─ 发送 ANNOUNCE (1Hz)      Slave1, Slave2
       │  优先级=128, ID=...       │
       │                           ├─ 接收 ANNOUNCE
       │                           ├─ BMC 算法 (我要跟这个 Master)
       │                           └─ 进入 LISTENING 状态
       │
 100ms ├─ 发送 SYNC (t1≈100ms)     │
       │                           ├─ 接收 SYNC (t2=100.023ms)
       │                           │  (网卡采集精确时间戳)
       │
       ├─ [HW 中断采集 Tx 时间]    │
       ├─ 发送 FOLLOW_UP           ├─ 接收 FOLLOW_UP
       │  (精确 t1=100.000321ms)   │  (读到精确发送时间)
       │                           │
       │                           ├─ [计算延迟]
       │                           └─ t2 - t1 = 23µs
       │
 200ms │ (同步周期继续...)          │
       │                           ├─ 发送 DELAY_REQ (t3=200.100ms)
       │                           │
       ├─ 接收 DELAY_REQ (t4=...)  │
       ├─ 发送 DELAY_RESP (t4=...) │
       │                           ├─ 接收 DELAY_RESP (t5)
       │                           │  计算单向延迟
       │                           │
       │                           ├─ [计算时间误差]
       │                           │  offset = (t2-t1+t4-t5)/2
       │                           │        = +15µs (快了 15µs)
       │                           │
       │                           ├─ [PI Servo 调整]
       │                           │  freq_adj = -5 PPM
       │                           │  (减速 5µs/s)
       │                           │
       │                           └─ 同步状态 = 已同步 ✅
       │
 300ms ├─ 发送 SYNC (t1≈300ms)    │
       │                           ├─ 接收 (offset = +5µs)
       │                           └─ 继续调整...
```

---

## 第五部分：关键实现细节

### 5.1 序列号匹配

为了配对多个消息，使用 `sequence_id`：

```c
// 发送 SYNC 时
sync_msg.sequence_id = ptp_state.seq_counter++;
ptp_state.pending_sync[sync_msg.sequence_id] = (pending_sync_t) {
    .seq_id = sync_msg.sequence_id,
    .origin_timestamp = sync_msg.origin_timestamp,
    .tx_timestamp = UNKNOWN,  // 等待硬件时间戳
};

// 接收 FOLLOW_UP 时
follow_up_msg.sequence_id == pending_sync[seq_id].seq_id  // 匹配!

// 类似地，DELAY_REQ 和 DELAY_RESP 也通过 sequence_id 配对
```

### 5.2 时间戳精度

| 时间戳来源 | 精度 | 采集方式 | 延迟 |
|-----------|------|--------|------|
| 硬件时间戳 (Tx) | ±1ns | 网卡在报文发送时采集 | <1µs |
| 硬件时间戳 (Rx) | ±1ns | 网卡在报文接收时采集 | <1µs |
| 软件时间戳 | ±100ns | 中断处理程序读 CPU 时钟 | 10-100µs |
| 系统时钟 (软件) | ±10µs | 普通读取 | 不稳定 |

**最佳实践**：
- 优先使用硬件时间戳 (精度高 1000 倍)
- 次选软件时间戳在中断中采集 (在硬件中断中, 延迟最小)
- 避免在主线程中读系统时钟 (延迟不确定)

### 5.3 网络对称性假设

```
假设: 上行延迟 = 下行延迟
  Master → Slave = 23µs
  Slave → Master = 23µs
  
现实:
  如果不对称，会导致同步误差
  例: 上行 20µs, 下行 30µs → 误差 ±5µs
  
解决:
  1. 使用高质量网络设备 (交换机而非 Hub)
  2. 在 DELAY_REQ 中添加补偿参数
  3. 定期测量并校正
```

---

## 参考资源

1. **IEEE 1588-2019 标准** - PTP 协议完整定义
2. **Linux ptp4l** - 开源 PTP 实现参考
3. **硬件时间戳** - 各 SOC 的时间戳寄存器使用

---

**文档版本**: v1.0  
**最后更新**: 2026-01-20  
**难度等级**: ⭐⭐⭐ (中等)  
**预计学习时间**: 4-6 小时
