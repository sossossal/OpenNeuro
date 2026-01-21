# PTP 时间同步 - 详细技术设计文档

**文档版本**: v1.0  
**日期**: 2026 年 1 月 20 日  
**状态**: 设计阶段  
**所有者**: OpenNeuro PTP 工作组  

---

## 📌 概述

本文档定义了在 OpenNeuro 系统中实现 IEEE 1588 v2 Precision Time Protocol (PTP) 的详细技术方案，
目标是在分布式机器人系统中实现 **<10 微秒的时间同步精度**，
支持多个异构平台 (ESP32, STM32H7, RK3588, Jetson, PC)。

### 核心目标

| 指标 | 目标值 | 重要性 | 备注 |
|------|--------|--------|------|
| **同步精度** | <10 µs (P99) | ⭐⭐⭐⭐⭐ | 硬实时控制的基础 |
| **稳定性** | >99.9% | ⭐⭐⭐⭐⭐ | 24/7 连续运行 |
| **支持节点数** | >16 个 | ⭐⭐⭐⭐ | 可扩展性 |
| **锁定时间** | <30 秒 | ⭐⭐⭐⭐ | 快速启动 |
| **代码大小** | <50 KB | ⭐⭐⭐ | 嵌入式资源限制 |

---

## 🏗️ 架构总览

### 系统分层

```
┌─────────────────────────────────────────────┐
│ 应用层 (Application)                         │
│ - Zenoh Publisher/Subscriber                │
│ - ROS 2 Nodes                              │
│ - 用户代码                                   │
├─────────────────────────────────────────────┤
│ PTP 适配层 (Synchronization Adaptation)     │
│ - ptp_get_time()  → 获取同步的全局时间     │
│ - ptp_wait_until() → 等待特定时刻           │
│ - ptp_set_callback() → 在精确时间执行回调  │
├─────────────────────────────────────────────┤
│ PTP 核心 (PTP Clock)                        │
│ - Master Clock State Machine               │
│ - Slave Clock Servo (PI-control)          │
│ - Event Message Processing                │
├─────────────────────────────────────────────┤
│ 硬件适配层 (Hardware Adaptation)            │
│ - 硬件时间戳 (Ethernet MAC)                │
│ - 高精度定时器 (SysTick, PIT)              │
│ - 时钟源驱动 (XTAL, PLL)                  │
├─────────────────────────────────────────────┤
│ 物理层 (Physical Layer)                     │
│ - Ethernet MAC/PHY                         │
│ - WiFi 无线电                               │
└─────────────────────────────────────────────┘
```

### 主要组件

```
PTP 协议栈
├── libptp_core/
│   ├── ptp_master.c      (Master 状态机)
│   ├── ptp_slave.c       (Slave 状态机)
│   ├── ptp_clock.c       (时钟管理)
│   ├── ptp_servo.c       (PI 控制器)
│   └── ptp_messages.c    (消息编解码)
├── libptp_hw/
│   ├── stm32h7_ptp_hw.c  (STM32 时间戳)
│   ├── rk3588_ptp_hw.c   (RK3588 时间调整)
│   ├── esp32_ptp_hw.c    (ESP32 软件时间戳)
│   └── ptp_hw_abstract.h (硬件抽象层)
└── libptp_api/
    ├── ptp_api.h         (公开 API)
    ├── ptp_config.h      (配置参数)
    └── ptp_platform.h    (平台定义)
```

---

## 🔄 PTP 协议流程

### Master-Slave 同步周期

```
Master                          Slave
  │                              │
  ├─ SYNC (t1)                  │
  ├─────────────────────────────→│ (接收时间 t2)
  │                              │
  ├─ FOLLOW_UP (t1)             │
  ├─────────────────────────────→│ (计算偏差)
  │                              │
  │                        DELAY_REQ (t3)
  │                         ←─────────────┤
  │                              │
  ├─ DELAY_RESP (t4)            │
  ├─────────────────────────────→│ (校准时钟)
  │                              │
  └─ (重复每 1 秒)              │
```

### 关键时间点

```
时间戳定义:
  t1 = Master 发出 SYNC 的时间
  t2 = Slave 收到 SYNC 的时间  
  t3 = Slave 发出 DELAY_REQ 的时间
  t4 = Master 收到 DELAY_REQ 的时间

计算:
  单向延迟 (OWD) = [(t2 - t1) + (t4 - t3)] / 2
  时间偏差 (offset) = (t2 - t1) - OWD
  
纠正:
  slave_clock_adjustment = -offset
```

---

## 💾 数据结构设计

### PTP 时钟状态

```c
// ptp_clock.h

/**
 * PTP 时间戳 (IEEE 1588 格式)
 * 秒精度到纳秒
 */
typedef struct {
    uint32_t seconds;           // 秒数 (Unix epoch 相对)
    uint32_t nanoseconds;       // 纳秒 (0-999,999,999)
} ptp_timestamp_t;

/**
 * PTP 时钟状态
 */
typedef struct {
    // 时间信息
    ptp_timestamp_t current_time;      // 当前同步时间
    int64_t offset_ns;                 // 与 master 的偏差 (纳秒)
    int32_t freq_offset_ppb;           // 频率偏差 (ppm)
    
    // 同步统计
    uint32_t sync_interval_ms;         // SYNC 间隔 (毫秒)
    uint32_t sync_count;               // 已同步次数
    uint32_t last_sync_age_ms;         // 最后同步时间差
    
    // 质量指标
    uint32_t steps_removed;            // 距离 grandmaster 的步数
    uint8_t clock_class;               // 时钟等级 (6=lock, 7=unsynced)
    uint8_t clock_accuracy;            // 精度等级
    
    // 状态机
    uint8_t port_state;                // PORT_DISABLED/INITIALIZING/FAULTY/LISTENING/PRE_MASTER/MASTER/PASSIVE/SLAVE/UNCALIBRATED/SLAVE
} ptp_clock_state_t;

/**
 * PTP 事件消息 (SYNC, DELAY_REQ)
 * 精确到纳秒
 */
typedef struct {
    ptp_timestamp_t timestamp;         // 事件时间戳
    uint64_t sequence_id;              // 序列号 (用于配对)
    uint8_t message_type;              // SYNC / DELAY_REQ / FOLLOW_UP / DELAY_RESP
    
    // 可选字段
    int16_t correction_field;          // 累积校正值 (纳秒)
    uint8_t domain_number;             // PTP 域 (0-127)
} ptp_event_message_t;

/**
 * PTP 通用消息 (FOLLOW_UP, DELAY_RESP)
 * 包含时间戳细节
 */
typedef struct {
    ptp_timestamp_t precise_origin_timestamp;
    int64_t correction_field;          // 纳秒
    uint32_t source_port_identity;
} ptp_general_message_t;

/**
 * PI 伺服控制器 (用于时钟调整)
 */
typedef struct {
    // PID 参数 (固定)
    int32_t kp;                        // 比例增益
    int32_t ki;                        // 积分增益
    int32_t kd;                        // 微分增益
    
    // 积分器状态
    int64_t integral;                  // 积分累积值
    int64_t last_error;                // 上次偏差
    
    // 限制
    int32_t max_freq_adjust;           // 最大频率调整 (ppb)
    int32_t max_step_adjust;           // 最大步进调整 (ns)
} ptp_servo_t;
```

---

## 🎯 Master 节点实现

### Master 状态机

```
DISABLED
  │
  ├─ ptp_start() → INITIALIZING
  │
INITIALIZING
  │ (选举或配置)
  │
  ├─ 成为 Master → MASTER
  │
MASTER
  │
  ├─ 定期生成 SYNC 消息 (每秒 10 次 = 100ms)
  │
  ├─ 处理进入的 DELAY_REQ 消息
  │ └─ 发送 DELAY_RESP + 时间戳
  │
  └─ (继续循环)
```

### Master 核心代码框架

```c
// libptp_core/ptp_master.c

typedef struct {
    ptp_clock_state_t clock;
    uint32_t sync_interval_ms;         // SYNC 消息间隔
    uint32_t domain_number;
    uint8_t priority1, priority2;      // Best Master Clock (BMC) 算法参数
    
    // 发送统计
    uint32_t sync_messages_sent;
    uint32_t delay_resp_sent;
} ptp_master_ctx_t;

/**
 * 初始化 PTP Master
 */
int ptp_master_init(ptp_master_ctx_t *master, 
                    uint32_t sync_interval_ms) {
    master->clock.current_time.seconds = time(NULL);
    master->clock.current_time.nanoseconds = 0;
    master->clock.clock_class = 6;     // LOCKED
    master->clock.port_state = PTP_STATE_MASTER;
    master->sync_interval_ms = sync_interval_ms;
    
    // 启动定期 SYNC 消息发送任务
    xTaskCreate(ptp_master_sync_task, ..., master);
    
    return 0;
}

/**
 * Master 主循环任务
 */
static void ptp_master_sync_task(void *arg) {
    ptp_master_ctx_t *master = (ptp_master_ctx_t *)arg;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    
    while (1) {
        // 等待同步间隔
        vTaskDelayUntil(&xLastWakeTime, 
                       pdMS_TO_TICKS(master->sync_interval_ms));
        
        // 获取精确时间戳
        ptp_hw_get_timestamp(&master->clock.current_time);
        
        // 发送 SYNC 消息
        ptp_event_message_t sync_msg = {
            .timestamp = master->clock.current_time,
            .message_type = PTP_MESSAGE_SYNC,
            .sequence_id = master->sync_messages_sent++,
        };
        ptp_master_send_sync(master, &sync_msg);
        
        // 发送 FOLLOW_UP 消息 (包含精确时间戳)
        ptp_general_message_t follow_up = {
            .precise_origin_timestamp = sync_msg.timestamp,
        };
        ptp_master_send_follow_up(master, &follow_up);
    }
}

/**
 * 处理来自 Slave 的 DELAY_REQ 消息
 */
void ptp_master_handle_delay_req(ptp_master_ctx_t *master,
                                  const ptp_event_message_t *delay_req) {
    // 获取接收时间 (t4 in 协议)
    ptp_timestamp_t rx_time;
    ptp_hw_get_timestamp(&rx_time);
    
    // 发送 DELAY_RESP
    ptp_general_message_t delay_resp = {
        .precise_origin_timestamp = rx_time,
        .source_port_identity = master->port_identity,
    };
    ptp_master_send_delay_resp(master, &delay_resp);
    
    master->delay_resp_sent++;
}
```

---

## 🔗 Slave 节点实现

### Slave 状态机

```
DISABLED
  │
  ├─ ptp_start() → INITIALIZING/LISTENING
  │
LISTENING
  │ (等待发现 Master)
  │
  ├─ 收到 ANNOUNCE → UNCALIBRATED
  │
UNCALIBRATED
  │ (开始同步过程)
  │
  ├─ 收到 SYNC/FOLLOW_UP → SLAVE
  │
SLAVE
  │
  ├─ 定期接收 SYNC 消息
  │
  ├─ 计算时间偏差
  │ └─ 调整本地时钟 (PI 伺服)
  │
  └─ (继续循环，每秒 10 次)
```

### Slave 核心代码框架

```c
// libptp_core/ptp_slave.c

typedef struct {
    ptp_clock_state_t clock;
    
    // 同步跟踪
    ptp_timestamp_t last_sync_time;
    ptp_timestamp_t last_follow_up_time;
    int64_t last_offset_ns;
    
    // PI 伺服
    ptp_servo_t servo;
    
    // 统计
    uint32_t sync_messages_received;
    uint32_t delay_resp_received;
    uint32_t synchronization_locked;
} ptp_slave_ctx_t;

/**
 * 初始化 PTP Slave
 */
int ptp_slave_init(ptp_slave_ctx_t *slave) {
    slave->clock.clock_class = 7;      // UNSYNCED
    slave->clock.port_state = PTP_STATE_LISTENING;
    
    // 初始化 PI 伺服参数
    slave->servo.kp = 1000;            // 比例增益
    slave->servo.ki = 100;             // 积分增益
    slave->servo.integral = 0;
    slave->servo.max_freq_adjust = 1000000;  // ±1000 ppm
    
    // 启动监听任务
    xTaskCreate(ptp_slave_listen_task, ..., slave);
    
    return 0;
}

/**
 * 处理接收的 SYNC 消息
 */
void ptp_slave_handle_sync(ptp_slave_ctx_t *slave,
                           const ptp_event_message_t *sync) {
    // 保存接收时间 (t2)
    ptp_hw_get_timestamp(&slave->last_sync_time);
    
    // 保存 SYNC 消息信息供 FOLLOW_UP 配对
    // (在实际实现中需要配队列等)
}

/**
 * 处理 FOLLOW_UP 消息 (包含 Master 精确时间戳 t1)
 */
void ptp_slave_handle_follow_up(ptp_slave_ctx_t *slave,
                                const ptp_general_message_t *follow_up) {
    // t1: Master 的精确发送时间
    ptp_timestamp_t t1 = follow_up->precise_origin_timestamp;
    
    // t2: Slave 收到的时间 (之前保存)
    ptp_timestamp_t t2 = slave->last_sync_time;
    
    // 现在需要 t3, t4 来计算单向延迟
    // (这需要 DELAY_REQ/DELAY_RESP 交互)
    
    // 暂时使用假设延迟 = 0
    int64_t delay_ns = 0;  // 应该从 DELAY_RESP 获取
    
    // 计算时间偏差
    int64_t offset_ns = ptp_timestamp_to_ns(&t2) - 
                       ptp_timestamp_to_ns(&t1) - delay_ns;
    
    // 使用 PI 伺服调整时钟
    ptp_slave_servo_update(&slave->servo, offset_ns);
    
    slave->last_offset_ns = offset_ns;
    slave->sync_messages_received++;
    
    // 如果偏差接近 0，标记为已同步
    if (llabs(offset_ns) < 1000000) {  // <1µs
        if (!slave->synchronization_locked) {
            slave->synchronization_locked = 1;
            slave->clock.clock_class = 6;  // LOCKED
            LOG_INFO("PTP: 时钟已锁定，偏差 = %lld ns", offset_ns);
        }
    }
}

/**
 * 发送 DELAY_REQ 消息以测量网络延迟
 */
void ptp_slave_send_delay_req(ptp_slave_ctx_t *slave) {
    ptp_timestamp_t t3;
    ptp_hw_get_timestamp(&t3);
    
    ptp_event_message_t delay_req = {
        .timestamp = t3,
        .message_type = PTP_MESSAGE_DELAY_REQ,
        .sequence_id = slave->sync_messages_received,
    };
    
    // 发送消息
    ptp_sock_send(&delay_req);
}

/**
 * 处理 DELAY_RESP 消息
 */
void ptp_slave_handle_delay_resp(ptp_slave_ctx_t *slave,
                                 const ptp_general_message_t *delay_resp) {
    // t4: Master 收到 DELAY_REQ 的时间
    ptp_timestamp_t t4 = delay_resp->precise_origin_timestamp;
    
    // 已有 t3 和 t4，可计算单向延迟
    // 单向延迟 = [(t2-t1) + (t4-t3)] / 2
    // (这是简化的计算，完整的需要累积校正字段)
}
```

### PI 伺服控制器

```c
// libptp_core/ptp_servo.c

/**
 * 更新 PI 伺服并返回频率调整值
 * 返回值: 频率调整 (ppb, parts per billion)
 */
int32_t ptp_servo_update(ptp_servo_t *servo, int64_t error_ns) {
    // PI 控制: output = Kp * error + Ki * integral(error)
    
    // 比例项
    int64_t p_term = (int64_t)servo->kp * error_ns;
    
    // 积分项 (积分累积)
    servo->integral += error_ns;
    if (servo->integral > 1000000000000LL) {
        servo->integral = 1000000000000LL;  // 防止溢出
    }
    int64_t i_term = (int64_t)servo->ki * servo->integral;
    
    // 合并
    int64_t output = (p_term + i_term) / 1000000;
    
    // 限制输出范围
    if (output > servo->max_freq_adjust) {
        output = servo->max_freq_adjust;
    }
    if (output < -servo->max_freq_adjust) {
        output = -servo->max_freq_adjust;
    }
    
    // 应用到硬件时钟
    ptp_hw_adjust_clock_frequency((int32_t)output);
    
    return (int32_t)output;
}
```

---

## ⚙️ 硬件时间戳实现

### STM32H7 以太网时间戳

STM32H7 以太网 MAC 支持硬件时间戳，精度为纳秒级。

```c
// firmware/zenoh-pico-stm32/src/ptp_hw.c

#include "stm32h7xx_hal.h"

// 全局配置
static uint64_t ptp_clock_offset_ns = 0;

/**
 * 初始化 PTP 硬件单元
 */
void ptp_hw_init(void) {
    // 配置以太网时间戳
    
    // 1. 启用精细时间更新 (Fine Update Mode)
    ETH->PTPTSCR &= ~ETH_PTPTSCR_TSFCU;  // 清除粗更新标志
    ETH->PTPTSCR |= ETH_PTPTSCR_TSFCU;   // 启用精细更新
    
    // 2. 初始化时间戳
    ETH->PTPTSHR = 0;  // 秒寄存器
    ETH->PTPTSLR = 0;  // 纳秒寄存器
    
    // 3. 启用 PTP 时间戳
    ETH->PTPTSCR |= ETH_PTPTSCR_TSE;
    
    // 4. 配置子秒增量 (每个时钟周期增加多少纳秒)
    // 系统时钟 = 480 MHz = 2.083 ns 周期
    // 我们希望每秒添加 1,000,000,000 ns
    // 增量 = 1,000,000,000 / (480*1,000,000) ≈ 2
    ETH->PTPTSAR = 2;
}

/**
 * 获取精确时间戳 (Ingress)
 */
void ptp_hw_get_rx_timestamp(uint32_t *sec, uint32_t *nsec) {
    *sec = ETH->PTPRXTS1R;   // 接收时间戳秒
    *nsec = ETH->PTPRXTS2R;  // 接收时间戳纳秒
}

/**
 * 获取精确时间戳 (Egress)
 */
void ptp_hw_get_tx_timestamp(uint32_t *sec, uint32_t *nsec) {
    *sec = ETH->PTPTXTS1R;   // 发送时间戳秒
    *nsec = ETH->PTPTXTS2R;  // 发送时间戳纳秒
}

/**
 * 通过调整子秒增量来微调频率
 * freq_adj: ppm (parts per million)
 */
void ptp_hw_adjust_frequency(int32_t freq_adj) {
    // 基础增量 ≈ 2 (见上面)
    // 调整 = 基础 × (1 + freq_adj / 1,000,000)
    uint32_t addend = 2 * (1000000 + freq_adj) / 1000000;
    ETH->PTPTSAR = addend;
}

/**
 * 步进调整时钟 (快速同步)
 */
void ptp_hw_step_clock(int64_t offset_ns) {
    if (offset_ns > 0) {
        // 向前调整
        uint32_t sec = offset_ns / 1000000000LL;
        uint32_t nsec = offset_ns % 1000000000LL;
        ETH->PTPTSINUR = nsec;
        ETH->PTPTSINSR = sec;
        ETH->PTPTSCR |= ETH_PTPTSCR_TSARU;  // 执行调整
    } else {
        // 向后调整
        offset_ns = -offset_ns;
        uint32_t sec = offset_ns / 1000000000LL;
        uint32_t nsec = offset_ns % 1000000000LL;
        ETH->PTPTSINUR = nsec | 0x80000000;  // 减法标志
        ETH->PTPTSINSR = sec;
        ETH->PTPTSCR |= ETH_PTPTSCR_TSARU;  // 执行调整
    }
}

/**
 * 读取当前时间
 */
uint64_t ptp_hw_get_time_ns(void) {
    uint32_t sec = ETH->PTPTSHR;
    uint32_t nsec = ETH->PTPTSLR & 0x7FFFFFFF;
    return (uint64_t)sec * 1000000000ULL + nsec + ptp_clock_offset_ns;
}
```

### RK3588 系统时间同步

在 Linux 系统上，使用内核 PTP 接口：

```c
// firmware/zenoh-pico-rk3588/src/ptp_hw.c

#include <sys/timex.h>
#include <time.h>

/**
 * 微调时钟频率
 */
void ptp_hw_adjust_frequency(int32_t freq_adj) {
    struct timex tx = {};
    
    tx.modes = ADJ_FREQUENCY;
    tx.freq = freq_adj * 65536;  // 转换为内核单位
    
    if (adjtimex(&tx) < 0) {
        perror("adjtimex");
    }
}

/**
 * 快速步进调整
 */
void ptp_hw_step_clock(int64_t offset_ns) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    
    // 加上偏差
    ts.tv_nsec += offset_ns;
    if (ts.tv_nsec >= 1000000000) {
        ts.tv_sec += 1;
        ts.tv_nsec -= 1000000000;
    } else if (ts.tv_nsec < 0) {
        ts.tv_sec -= 1;
        ts.tv_nsec += 1000000000;
    }
    
    if (clock_settime(CLOCK_REALTIME, &ts) < 0) {
        perror("clock_settime");
    }
}
```

### ESP32 软件时间戳

```c
// firmware/zenoh-pico-esp32/src/ptp_hw.c

#include "freertos/FreeRTOS.h"
#include "driver/gptimer.h"

static gptimer_handle_t gptimer = NULL;

/**
 * 初始化高精度定时器 (General Purpose Timer)
 */
void ptp_hw_init(void) {
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_APB,     // 80 MHz APB 时钟
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000,            // 1 MHz 分辨率 = 1µs
    };
    
    gptimer_new_timer(&timer_config, &gptimer);
    gptimer_enable(gptimer);
    gptimer_start(gptimer);
}

/**
 * 获取当前微秒级时间
 */
uint64_t ptp_hw_get_time_us(void) {
    uint64_t count;
    gptimer_get_count(gptimer, &count);
    return count;  // 1µs 分辨率
}

/**
 * 微调频率 (软件 PPM 调整)
 */
void ptp_hw_adjust_frequency(int32_t freq_adj) {
    // ESP32 没有直接的硬件频率调整
    // 可以通过调整计时器分频系数实现
    // 但简化起见，可以使用软件时间修正
    
    // 实现: 每秒额外调整 N 微秒
    // adjustment_us_per_sec = (freq_adj / 1,000,000) * 1,000,000
    //                       = freq_adj
}
```

---

## 📡 网络消息格式

### 以太网帧格式

PTP 消息直接在 UDP 上传输 (IEEE 1588-2008):

```
以太网帧:
├─ 目标 MAC: 01-1B-19-00-00-00 (PTP 组播)
├─ 源 MAC: [设备 MAC]
├─ EtherType: 0x0800 (IPv4)
│
IPv4 报头:
├─ 源 IP: [设备 IP]
├─ 目标 IP: 224.0.1.129 (PTP 组播)
│
UDP 报头:
├─ 源端口: 319 (PTP Event) 或 320 (PTP General)
├─ 目标端口: 319 或 320
│
PTP 消息体 (最少 34 字节):
├─ 版本 (2 字节): 0x0200 (版本 2)
├─ 消息类型 (1 字节): 0=SYNC, 1=DELAY_REQ, ...
├─ 长度 (2 字节): 消息总长度
├─ 域号 (1 字节): 0-127 (0 = 默认)
├─ 保留 (1 字节): 0x00
├─ 标志 (2 字节): 校准, 一步...
├─ 校正字段 (8 字节): 累积延迟 (纳秒)
├─ 源时钟标识 (10 字节): 时钟唯一标识
├─ 序列号 (2 字节): 消息序列
├─ 控制 (1 字节): SYNC/FOLLOW_UP/DELAY_REQ/DELAY_RESP
└─ 日志间隔 (1 字节): 消息间隔指数
```

### SYNC 消息体 (Event)

```c
typedef struct __packed {
    uint8_t version[2];                // 0x0200
    uint8_t message_type_reserved;     // 0x00 (SYNC)
    uint8_t message_length[2];         // 44 字节
    uint8_t domain_number;             // 0
    uint8_t reserved;
    uint8_t flags[2];
    uint8_t correction_field[8];       // 0
    uint8_t clock_identity[8];         // Master 时钟标识
    uint8_t source_port_number[2];     // 端口号 (通常 1)
    uint8_t sequence_id[2];            // 序列号
    uint8_t control;                   // 0x00 (SYNC)
    uint8_t log_message_interval;      // -3 (1/8 秒间隔)
    
    // 有效载荷
    uint8_t origin_timestamp[10];      // t1: Master 发送时间
} __packed ptp_sync_message_t;
```

---

## 📊 性能目标与验收标准

### 同步精度指标

```
指标                 目标          P50      P95      P99
────────────────────────────────────────────────────────
单节点偏差           <10 µs         <3µs     <7µs     <10µs
多节点同步偏差       <50 µs         <20µs    <40µs    <50µs
快速锁定时间         <30 秒         20s      28s      30s
长期漂移             <1 µs/分钟     -        -        -
网络抖动恢复         <5 秒          -        -        -
```

### 功耗和资源限制

```
指标              限制          现实估计
──────────────────────────────────────
ROM 大小          <50 KB        ~45 KB
RAM 使用          <10 KB        ~8 KB
CPU 占用率        <2%           ~1%
网络带宽 (PTP)    <100 kbps     ~80 kbps
```

---

## 🧪 测试计划

### 单元测试

```
✓ 时间戳编解码
✓ PI 伺服算法
✓ 状态机转换
✓ 消息序列化
✓ 硬件适配层接口
```

### 集成测试

```
✓ Master-Single Slave
✓ Master-多个 Slaves (最少 4 个)
✓ 跨网络 (Ethernet + WiFi)
✓ 长期稳定性 (24 小时)
✓ 故障转移 (Master 宕机)
```

### 性能测试

```
✓ 同步精度 (100 样本)
✓ 锁定时间测量
✓ CPU 占用率
✓ 内存峰值
✓ 网络延迟分布
```

---

## 🔄 集成与部署

### 与 Zenoh 的集成

```
Zenoh Router
    ↓
[PTP Master 运行在 Router]
    ↓ SYNC/FOLLOW_UP 组播
    ↓ (通过 Zenoh 原生网络)
Zenoh Clients (都同步)
```

### 与 ROS 2 的集成

```
ROS 2 Time Server
    │
    ├─ 读取 ptp_get_time() 获取全局时间
    │
    └─ 所有 ROS 节点使用全局 PTP 时间
        (而不是本地系统时间)
```

---

## 📋 实施检查清单

- [ ] PTP 协议栈核心实现
- [ ] Master 和 Slave 状态机
- [ ] 消息编解码
- [ ] STM32H7 硬件时间戳驱动
- [ ] RK3588 时钟调整驱动
- [ ] ESP32 软件时间戳实现
- [ ] PI 伺服控制器
- [ ] 网络消息传输
- [ ] 故障检测和恢复
- [ ] 日志和诊断工具
- [ ] 完整的 API 文档
- [ ] 使用示例代码
- [ ] 自动化测试套件
- [ ] 性能验证报告
- [ ] 部署和配置指南

---

**此文档为 Stage 2 PTP 子项目的详细技术规范**

下一步: 实现 PTP 协议栈核心代码

