# OpenNeuro API Reference
**Version**: v0.2.0-alpha.1  
**Date**: 2026-01-21

## 目录
1. [PTP Master API](#ptp-master-api)
2. [PTP Slave API](#ptp-slave-api)
3. [TSN Scheduler API](#tsn-scheduler-api)
4. [Zenoh Bridge API](#zenoh-bridge-api)
5. [示例代码](#examples)

---

## PTP Master API

### 初始化

```c
#include "master/master.h"

// 配置结构
typedef struct {
    uint8_t priority1;        // BMC 优先级 1 (默认: 128)
    uint8_t priority2;        // BMC 优先级 2 (默认: 128)
    uint8_t clock_class;      // 时钟类别 (默认: 248)
    uint8_t clock_accuracy;   // 时钟精度
    uint16_t clock_variance;  // 时钟方差
    uint8_t time_source;      // 时间源
    uint8_t domain_number;    // PTP 域号 (默认: 0)
    const char *iface;        // 网络接口名
} ptp_master_config_t;

// 初始化 Master
int ptp_master_init(ptp_master_ctx_t *ctx, const ptp_master_config_t *config);
```

### 主循环

```c
// 周期性调用 (例如每 10ms)
void ptp_master_tick(ptp_master_ctx_t *ctx);

// 处理接收到的数据包
void ptp_master_handle_packet(ptp_master_ctx_t *ctx, uint8_t *data, size_t len);
```

### 示例

```c
ptp_master_config_t cfg = {
    .priority1 = 128,
    .priority2 = 128,
    .clock_class = 248,
    .domain_number = 0,
    .iface = "eth0"
};

ptp_master_ctx_t master;
ptp_master_init(&master, &cfg);

// 主循环
while (running) {
    ptp_master_tick(&master);
    // 处理网络 I/O
    sleep_ms(10);
}
```

---

## PTP Slave API

### 初始化

```c
#include "slave/slave.h"

typedef struct {
    uint8_t domain_number;
    const char *iface;
    int delay_req_interval_log;  // Log2 间隔 (例如 -3 = 125ms)
} ptp_slave_config_t;

int ptp_slave_init(ptp_slave_ctx_t *ctx, const ptp_slave_config_t *config);
```

### 主循环

```c
void ptp_slave_tick(ptp_slave_ctx_t *ctx);
void ptp_slave_handle_packet(ptp_slave_ctx_t *ctx, uint8_t *data, size_t len);
```

### 状态查询

```c
// 获取当前同步状态
ptp_slave_state_t state = ctx->state;
// PTP_SLAVE_LISTENING, PTP_SLAVE_UNCALIBRATED, PTP_SLAVE_SLAVE

// 获取时间偏移
int64_t offset_ns = ctx->offset_ns;
int64_t mean_delay_ns = ctx->mean_delay_ns;
```

---

## TSN Scheduler API

### 初始化

```c
#include "core/scheduler.h"

tsn_scheduler_t* tsn_scheduler_create(void);
void tsn_scheduler_destroy(tsn_scheduler_t *sched);
```

### 帧入队

```c
// 添加帧到指定优先级队列
int tsn_scheduler_enqueue(tsn_scheduler_t *sched, 
                          tsn_frame_t *frame, 
                          uint8_t priority);
```

### 调度

```c
// 选择下一个要发送的帧
tsn_frame_t* tsn_scheduler_select_next_frame(tsn_scheduler_t *sched, 
                                              uint64_t current_time_ns);
```

### 流量分类

```c
typedef enum {
    TSN_CLASS_RT = 0,  // 实时 (优先级 7,6)
    TSN_CLASS_ST = 1,  // 流媒体 (优先级 5,4)
    TSN_CLASS_CT = 2,  // 控制 (优先级 3,2)
    TSN_CLASS_BE = 3   // 尽力而为 (优先级 1,0)
} tsn_traffic_class_t;

tsn_traffic_class_t tsn_get_traffic_class(uint8_t priority);
```

---

## Zenoh Bridge API

### Zenoh HAL (抽象层)

```c
#include "zenoh_hal.h"

// 会话管理
zenoh_session_t* zenoh_hal_open(const char *locator);
void zenoh_hal_close(zenoh_session_t *session);

// 发布器
zenoh_publisher_t* zenoh_hal_declare_publisher(zenoh_session_t *s, const char *key);
int zenoh_hal_put(zenoh_publisher_t *pub, const uint8_t *data, size_t len);
void zenoh_hal_undeclare_publisher(zenoh_publisher_t *pub);

// 订阅器
typedef void (*zenoh_data_handler_t)(const uint8_t *data, size_t len, void *ctx);
zenoh_subscriber_t* zenoh_hal_declare_subscriber(zenoh_session_t *s, 
                                                   const char *key,
                                                   zenoh_data_handler_t handler, 
                                                   void *ctx);
```

### PTP 状态发布

```c
#include "ptp_publisher.h"

int ptp_publisher_init(zenoh_session_t *session);
void ptp_publisher_publish_status(int64_t offset_ns, uint32_t sync_count, uint8_t state);
void ptp_publisher_cleanup(void);
```

### TSN-Zenoh QoS 映射

```c
#include "tsn_qos_bridge.h"

// TSN → Zenoh 优先级映射
zenoh_priority_t tsn_to_zenoh_priority(tsn_traffic_class_t tsn_class);

// Zenoh → TSN 流量类别映射
tsn_traffic_class_t zenoh_to_tsn_class(zenoh_priority_t zenoh_prio);

// 拥塞控制策略
int tsn_get_congestion_control(tsn_traffic_class_t tsn_class);
// 返回: 0 = DROP, 1 = BLOCK
```

---

## 示例代码

### 完整 PTP 同步示例

```c
#include "master/master.h"
#include "slave/slave.h"

int main() {
    // 初始化 Master
    ptp_master_config_t master_cfg = {
        .priority1 = 128,
        .domain_number = 0,
        .iface = "eth0"
    };
    ptp_master_ctx_t master;
    ptp_master_init(&master, &master_cfg);
    
    // 初始化 Slave
    ptp_slave_config_t slave_cfg = {
        .domain_number = 0,
        .iface = "eth1",
        .delay_req_interval_log = -3
    };
    ptp_slave_ctx_t slave;
    ptp_slave_init(&slave, &slave_cfg);
    
    // 主循环
    while (1) {
        ptp_master_tick(&master);
        ptp_slave_tick(&slave);
        
        // 网络 I/O 处理...
        
        // 检查同步状态
        if (slave.state == PTP_SLAVE_SLAVE) {
            printf("Synchronized! Offset: %lld ns\n", slave.offset_ns);
        }
        
        sleep_ms(10);
    }
}
```

### TSN + Zenoh 集成示例

```c
#include "core/scheduler.h"
#include "zenoh_hal.h"
#include "tsn_qos_bridge.h"

int main() {
    // 初始化 TSN 调度器
    tsn_scheduler_t *sched = tsn_scheduler_create();
    
    // 初始化 Zenoh
    zenoh_session_t *session = zenoh_hal_open("tcp/127.0.0.1:7447");
    zenoh_publisher_t *pub = zenoh_hal_declare_publisher(session, "robot/sensor/imu");
    
    // 发送数据
    uint8_t data[100];
    // ... 填充数据 ...
    
    // 根据 TSN 类别映射 Zenoh 优先级
    tsn_traffic_class_t tsn_class = TSN_CLASS_RT;
    zenoh_priority_t zenoh_prio = tsn_to_zenoh_priority(tsn_class);
    
    // 发布 (实际 Zenoh API 会使用优先级)
    zenoh_hal_put(pub, data, sizeof(data));
    
    // 清理
    zenoh_hal_undeclare_publisher(pub);
    zenoh_hal_close(session);
    tsn_scheduler_destroy(sched);
}
```

---

## 编译选项

### CMake 配置

```bash
# 使用完整 Zenoh (默认)
cmake -DUSE_ZENOH_PICO=OFF ..

# 使用 Zenoh-Pico (嵌入式)
cmake -DUSE_ZENOH_PICO=ON ..
```

### 链接库

```cmake
target_link_libraries(your_app 
    ptp_core 
    tsn_scheduler 
    zenoh_bridge
)
```

---

## 性能指标

| 组件 | 指标 | 目标 | 实测 |
|------|------|------|------|
| PTP Master | TX 延迟 | <20µs | ~10ns |
| TSN Scheduler | 调度决策 | <100µs | O(1) |
| Zenoh HAL | Pub/Sub | N/A | Mock |

---

## 注意事项

1. **线程安全**: 当前实现不是线程安全的，需要外部同步
2. **网络 I/O**: API 不包含网络层，需要应用层实现
3. **硬件时间戳**: 当前使用软件时间戳，精度有限
4. **Zenoh Mock**: 当前 Zenoh 为 Mock 实现，需要链接真实库

---

## 下一步

- 查看 `ZENOH_INTEGRATION.md` 了解如何集成真实 Zenoh 库
- 查看 `PERFORMANCE_REPORT.md` 了解详细性能数据
- 查看 `examples/` 目录获取更多示例代码
