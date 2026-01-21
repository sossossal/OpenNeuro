# Zone Controller 硬件原理图设计指南

**版本**: v0.1.0  
**日期**: 2026-01-20  
**阶段**: Stage 2 Week 2  
**工具**: KiCad EDA  
**目标**: 定义原理图设计规范，准备 PCB 布局

---

## 📋 目录

1. [设计规范与标准](#设计规范与标准)
2. [电源子系统](#电源子系统)
3. [处理器与存储](#处理器与存储)
4. [网络接口](#网络接口)
5. [PTP 硬件时间戳](#ptp-硬件时间戳)
6. [扩展接口](#扩展接口)
7. [调试与监控](#调试与监控)
8. [KiCad 项目结构](#kicad-项目结构)
9. [W2-W4 设计计划](#w2-w4-设计计划)

---

## 设计规范与标准

### 设计约束

```
基本约束:
├─ 工作温度: 0°C - 50°C (工业级)
├─ 存储温度: -20°C - 70°C
├─ 相对湿度: 5% - 95% (非冷凝)
├─ 工作电压: 12V ±5% (外部)
├─ 功耗目标: <30W (持续), <50W (峰值)
├─ 外形尺寸: 150×100×50mm (轻度约束)
└─ 成本目标: ¥350-400 (BOM 单价)

安全标准:
├─ CE 标志 (EMC: EN 61000)
├─ RoHS 2.0 (铅免)
├─ ESD 保护: ±8kV 空气放电
└─ ECC 检查 (DDR 内存)

可靠性:
├─ MTBF: >50,000 小时
├─ 环境: IP54 (可选机械保护)
└─ 备用电源: 支持 RTC 电池 (可选)
```

### PCB 设计规则

```
层数: 4 层 (信号-GND-VCC-信号)
├─ Layer 1 (F.Cu): 信号与电源
├─ Layer 2 (GND): 完整 GND 平面
├─ Layer 3 (VCC): 3.3V 和 5V 分布
└─ Layer 4 (B.Cu): 信号与回程

线宽与间距:
├─ 信号线: 0.25mm (最小)
├─ 电源线: 0.5mm (12V), 0.3mm (3.3V/5V)
├─ 间距: 0.25mm (最小)

过孔设计:
├─ 直径: 0.3mm (最小)
├─ 焊盘: 0.5mm
├─ 蒂片: 0.3mm
└─ 通孔: 0.2mm

高速信号:
├─ Ethernet (RGMII): 阻抗 100Ω 差分
├─ PTP 时钟: 低抖动布线 (<50ps)
└─ DDR: 等长差分对
```

---

## 电源子系统

### 电源树设计

```
12V 输入 (2A 最大)
  │
  ├─ [输入保护] (保险丝 3A + TVS 二极管)
  │
  ├─→ [MP2107 降压器 #1]
  │    输入: 12V
  │    输出: 3.3V @ 5A
  │    ├─ RK3588 内核
  │    ├─ DDR 内存 (1.1V)
  │    └─ NAND Flash
  │
  ├─→ [MP2107 降压器 #2]
  │    输入: 12V
  │    输出: 5V @ 3A
  │    ├─ USB 3.0 Hub
  │    ├─ 以太网 PHY (1.8V)
  │    └─ 外设
  │
  └─→ [可选线性稳压]
       输入: 12V
       输出: 1.8V @ 1A (以太网模拟前端)
```

### MP2107 降压器配置

```c
/**
 * MP2107 降压器 PCB 设计指南
 * 芯片参数: 60V 输入, 3A 输出, 1MHz 开关频率
 */

// 降压器 #1 配置 (12V → 3.3V @ 5A)
typedef struct {
    float input_voltage = 12.0;   // V
    float output_voltage = 3.3;   // V
    float max_output_current = 5.0; // A
    float switching_frequency = 1.0e6;  // Hz (1MHz)
    
    // 计算反馈分压
    float r_feedback_top = 100e3;    // 100kΩ
    float r_feedback_bottom = 30e3;  // 30kΩ
    // Vout = 0.6 * (1 + R_top/R_bottom)
    // 3.3V = 0.6 * (1 + 100k/30k) ≈ 3.5V (调整)
    
    // 电感选择
    float inductance = 2.2e-6;  // 2.2µH (高频降压器)
    float inductance_ripple_pct = 40;  // 40% 纹波
    float peak_current = max_output_current * (1 + inductance_ripple_pct/200);
    
    // 输出电容
    float output_cap_total = 100e-6;  // 100µF (陶瓷)
    float output_cap_esr = 50e-3;     // 50mΩ (ESR)
    float output_ripple_voltage = 0.1; // 100mV
    
} mp2107_design_t;

// PCB 设计建议
/**
 * 1. 输入端:
 *    - 输入电容: 2x 100µF 陶瓷 + 10µF 钽电容 (低 ESR)
 *    - 放置于芯片旁边 (<1cm)
 *    - 独立返回到主 GND 平面
 *
 * 2. 电感:
 *    - 防止 EMI，使用屏蔽电感 2.2µH
 *    - 电感两端到输出电容的走线尽可能短
 *
 * 3. 输出端:
 *    - 输出电容: 4x 22µF 陶瓷 (C0G/X5R)
 *    - 1x 10µ tantalum (备份)
 *    - 分组放置于负载附近
 *
 * 4. 反馈网络:
 *    - R_feedback 放置于输出点附近
 *    - 使用高精度 1% 电阻
 *    - 远离干扰源
 *
 * 5. 地面返回:
 *    - 所有返回使用专用返回引脚
 *    - 最小化环路面积
 *    - 过孔间距 <5mm
 */
```

### 电源质量指标

| 指标 | 规格 | 测试条件 |
|------|------|---------|
| 输出纹波 | <100mV | 3A 负载，20MHz BW |
| 瞬态响应 | <50mV | 负载 0→3A，100ns 上升时间 |
| 效率 | >85% | 满负载，12V 输入 |
| 启动时间 | <10ms | 上电到稳定输出 |
| 关闭时序 | - | 软关闭 (可选) |

---

## 处理器与存储

### RK3588 SoM 集成

```
Orange Pi 5 Plus 模块
├─ RK3588 八核处理器 (4× Cortex-A76 + 4× Cortex-A55)
├─ 8GB LPDDR5 内存 (板载)
├─ 256GB eMMC (板载)
└─ 集成 GPU (Mali-G610)

主板连接:
├─ SO-DIMM 风格接口 (100 引脚)
├─ 2.5mm 间距连接器
└─ 3.3V I/O 电平
```

### 内存布局

```c
/**
 * RK3588 内存映射 (8GB 配置)
 */
typedef struct {
    // Bootloader 与 Kernel
    uint32_t sram_offset = 0x0000_0000;
    uint32_t sram_size = 512 * 1024;  // 512KB
    
    // DDR 内存
    uint32_t ddr_offset = 0x0010_0000;  // 256MB
    uint32_t ddr_size = 8 * 1024 * 1024 * 1024;  // 8GB
    
    // 内核分配
    uint32_t kernel_offset = 0x0040_0000;
    uint32_t kernel_size = 64 * 1024 * 1024;
    
    // 文件系统
    uint32_t rootfs_offset = 0x0440_0000;
    uint32_t rootfs_size = 3 * 1024 * 1024 * 1024;
    
    // 用户空间应用
    uint32_t userspace_offset = 0x1440_0000;
    uint32_t userspace_size = 4 * 1024 * 1024 * 1024;  // 剩余
    
} rk3588_memory_map_t;
```

### NAND Flash 设计

```
256GB eMMC 配置:
├─ Partition:
│  ├─ bootloader (4MB)
│  ├─ kernel (64MB)
│  ├─ device tree (4MB)
│  ├─ rootfs (128GB)
│  └─ userdata (剩余)
│
└─ 可靠性:
   ├─ ECC: 板载实现
   ├─ 备份分区: 支持 A/B 更新
   └─ 损耗均衡: NAND 控制器实现
```

---

## 网络接口

### 双 Gigabit 以太网设计

```
RK3588 内部 MAC (2 个)
  │
  ├─→ [MAC #1]
  │    │
  │    ├─→ RTL8211FD PHY #1
  │    │    ├─ 1G 自适应
  │    │    ├─ PTP 硬件时间戳
  │    │    └─ RMII 接口
  │    │
  │    └─→ RJ45 连接器 #1 (PTP 同步端口)
  │
  └─→ [MAC #2]
       │
       ├─→ RTL8211FD PHY #2
       │    ├─ 1G 自适应
       │    ├─ 可选 PTP
       │    └─ RMII 接口
       │
       └─→ RJ45 连接器 #2 (业务端口)
```

### RTL8211FD 配置

```c
/**
 * RTL8211FD Gigabit PHY
 * - IEEE 802.3 兼容
 * - RGMII/RMII 接口
 * - PTP 硬件时间戳支持
 */

typedef struct {
    // 基本配置
    uint8_t interface_mode = RGMII;  // RGMII 模式
    uint8_t auto_negotiation = true;
    uint16_t advertise_capabilities = 0x9E9F;  // 1G Full-Duplex
    
    // PTP 时间戳
    bool ptp_enabled = true;
    uint8_t ptp_version = 2;  // IEEE 1588 v2
    
    // 时钟延迟补偿
    int16_t tx_delay_ns = 0;   // 待测量
    int16_t rx_delay_ns = 0;   // 待测量
    
} rtl8211fd_config_t;

// 硬件连接
/**
 * RK3588 → RTL8211FD
 * 
 * RGMII 信号:
 * ├─ TX_CLK (输出)      → RTL8211FD TXC (1.8V)
 * ├─ TXD[3:0]           → RTL8211FD TXD[3:0]
 * ├─ TX_CTL             → RTL8211FD TX_EN
 * ├─ RX_CLK (输入)      → RTL8211FD RXC
 * ├─ RXD[3:0]           → RTL8211FD RXD[3:0]
 * ├─ RX_CTL             → RTL8211FD RX_DV
 * ├─ MDC (管理时钟)     → RTL8211FD MDC (1.8V)
 * ├─ MDIO (管理数据)    → RTL8211FD MDIO
 * └─ PHY_RST (复位)     → RTL8211FD RST_N
 */
```

### 时钟输出

```c
/**
 * PTP 参考时钟输出
 * - 输出: 同步时钟信号给从设备
 * - 精度: ±10ns
 */

// GPIO 配置
typedef struct {
    uint32_t gpio_base = 0xFEC60000;  // RK3588 GPIO4
    uint8_t gpio_pin = 5;
    bool inverted = false;
    
    // 时钟分配
    uint32_t clock_source = CLK_GMAC0;
    uint32_t clock_divider = 1;  // 1:1 (125MHz)
    
} ptp_clock_output_config_t;

// 时钟输出管脚:
// GPIO4_B5 (CN1 第 25 脚) → 50Ω 走线 → 同轴连接器 J3
```

---

## PTP 硬件时间戳

### 硬件实现架构

```
系统时钟
  │ (125 MHz, 来自 GMAC)
  │
  ├─→ [64 bit 计数器]
  │    ├─ 秒数部分 (32 bit)
  │    └─ 纳秒部分 (32 bit)
  │
  ├─→ [秒脉冲生成器]
  │    ├─ 1PPS 输出 (GPIO4_B4)
  │    └─ 中断信号
  │
  ├─→ [时间戳捕获]
  │    ├─ 发送时间戳 (Tx TS)
  │    ├─ 接收时间戳 (Rx TS)
  │    └─ GPIO 事件时间戳
  │
  └─→ [精度补偿]
       ├─ 固定延迟补偿
       └─ 温度补偿 (可选)
```

### 时间戳精度分析

```c
/**
 * PTP 时间戳精度指标
 */
typedef struct {
    uint32_t clock_frequency = 125000000;  // 125 MHz
    uint32_t clock_period_ns = 8;           // 1/125MHz = 8ns
    uint32_t timestamp_resolution_ns = 8;   // 8ns 精度
    
    // 网络延迟补偿
    int32_t egress_delay_ns = 48;    // 发送端口延迟 (PHY + 线路)
    int32_t ingress_delay_ns = 52;   // 接收端口延迟
    
    // 内部电路延迟
    int32_t mac_tx_delay_ns = 12;    // MAC 发送链路
    int32_t mac_rx_delay_ns = 14;    // MAC 接收链路
    
    // 总体精度 (实现目标)
    uint32_t accuracy_ns = 50;  // ±50ns 典型值
    
} ptp_hardware_precision_t;
```

### Rx/Tx 时间戳获取

```c
/**
 * 接收时间戳获取
 * 在 MAC 接收中断处理器中调用
 */
uint64_t ptp_get_rx_timestamp(const eth_rx_desc_t *rx_desc) {
    // 从硬件描述符获取时间戳
    uint64_t sec = rx_desc->timestamp_sec;
    uint32_t nsec = rx_desc->timestamp_nsec;
    
    return (sec << 32) | nsec;
}

/**
 * 发送时间戳获取
 * 在硬件发送完成中断后调用
 */
uint64_t ptp_get_tx_timestamp(const eth_tx_desc_t *tx_desc) {
    // 轮询 Tx TS 状态
    uint32_t timeout = 1000;  // 1ms
    while (timeout--) {
        if (tx_desc->timestamp_valid) {
            uint64_t sec = tx_desc->timestamp_sec;
            uint32_t nsec = tx_desc->timestamp_nsec;
            
            return (sec << 32) | nsec;
        }
        udelay(1);
    }
    
    return 0;  // 超时
}
```

---

## 扩展接口

### USB 3.0 (可选)

```
RK3588 USB 3.0 Controller
  │
  ├─→ [USB 3.0 Hub 芯片 (VL817)]
  │    可选, 提供额外 4 个 USB 3.0 端口
  │
  ├─→ USB Type-A 连接器 (Host)
  │    ├─ USB 3.0 @ 5Gbps
  │    ├─ USB 供电 (可选)
  │    └─ 防护二极管
  │
  └─→ USB Type-C 连接器 (设备)
       ├─ USB 2.0 HS (用于调试)
       └─ UART 转 USB (CH340 或 PL2303)
```

### GPIO 扩展 (40 引脚)

```
40 针 GPIO 接头 (Raspberry Pi 兼容)
├─ 3.3V 输出 (2个)
├─ 5V 输出 (2个)
├─ GND (6个)
├─ GPIO (共 26 个)
│  ├─ 通用 GPIO (12个)
│  ├─ I2C 接口 (2个)
│  ├─ SPI 接口 (4个)
│  └─ UART 接口 (2个)
└─ 特殊功能
   ├─ PWM (2个)
   └─ ADC (可选)
```

### 调试端口

```
┌─────────────────────────────────┐
│     调试接口配置                 │
├─────────────────────────────────┤
│ 串口 (UART0)   → USB 转 UART    │
│ JTAG (可选)    → 20 针连接器    │
│ 网络 (Telnet)  → Ethernet      │
└─────────────────────────────────┘
```

---

## 调试与监控

### 温度监控

```c
/**
 * RK3588 内部温度传感器
 */
typedef struct {
    uint32_t tsadc_base = 0xFEAC0000;
    
    // 温度阈值
    uint32_t warning_temp_c = 65;   // 警告
    uint32_t critical_temp_c = 85;  // 严重
    uint32_t shutdown_temp_c = 95;  // 关闭
    
    // 采样周期
    uint32_t sample_period_ms = 1000;  // 1秒采样一次
    
} thermal_monitor_config_t;

// 在内核启动时配置
void tsadc_init(void) {
    // 使能温度传感器
    uint32_t ctrl = readl(TSADC_BASE + TSADC_CTRL);
    ctrl |= TSADC_ENABLE;
    writel(ctrl, TSADC_BASE + TSADC_CTRL);
    
    // 设置中断阈值
    writel(WARN_TEMP, TSADC_BASE + TSADC_THRESH0);
    writel(CRIT_TEMP, TSADC_BASE + TSADC_THRESH1);
}
```

### 电压/电流监控 (可选)

```c
/**
 * INA226 电流/功率监控 IC
 * 用于实时监测功耗
 */
typedef struct {
    // I2C 配置
    uint8_t i2c_addr = 0x40;
    uint8_t i2c_bus = 1;
    
    // 分流电阻
    float shunt_resistor_ohm = 0.05;  // 50mΩ
    
    // 校准
    uint16_t calibration = 2048;  // 内部计算
    
} ina226_config_t;

// 读取功耗
uint32_t get_power_watts(void) {
    uint16_t power_reg = i2c_read16(INA226_ADDR, PWR_REG);
    return (power_reg * 25) / 1000;  // 转换为瓦特
}
```

---

## KiCad 项目结构

### 文件组织

```
zone-controller/hardware/
├── kicad/
│  ├── zone-controller.pro      # KiCad 项目文件
│  ├── zone-controller-sch.kicad_sch  # 原理图
│  ├── zone-controller-pcb.kicad_pcb  # PCB 布局
│  ├── footprints/
│  │  ├── RK3588-SoM.kicad_mod
│  │  ├── RTL8211FD.kicad_mod
│  │  ├── MP2107.kicad_mod
│  │  └── ... (其他)
│  ├── symbols/
│  │  ├── RK3588.kicad_sym
│  │  ├── RTL8211FD.kicad_sym
│  │  └── ... (其他)
│  └── 3d_models/
│     ├── connectors/
│     ├── ic_packages/
│     └── ... (3D 模型)
│
├── gerber/                      # PCB 生产文件
│  ├── *.gbr (Gerber 文件)
│  ├── *.gbl
│  ├── *.txt (钻孔文件)
│  └── *.drl
│
├── assembly/                    # 装配文件
│  ├── BOM_ASSEMBLY.csv
│  ├── PLACEMENT.csv
│  └── *.pdf (装配图)
│
├── documentation/
│  ├── schematic.pdf             # 原理图 PDF
│  ├── pcb_layout.pdf            # PCB 布局 PDF
│  ├── design_review.md          # 设计评审记录
│  └── test_points.pdf           # 测试点位置
│
└── manufacturing/
   ├── stackup.txt               # 层堆叠
   ├── trace_width.txt           # 线宽规则
   └── fab_notes.txt             # 制造说明
```

### 原理图分层

```
zone-controller.kicad_sch
├─ Sheet 1: 电源系统
│  ├─ 输入保护
│  ├─ 12V → 3.3V 降压
│  └─ 12V → 5V 降压
│
├─ Sheet 2: 处理器与存储
│  ├─ RK3588 SoM 接口
│  ├─ DDR 总线 (接线端子)
│  └─ eMMC 接口
│
├─ Sheet 3: 网络接口
│  ├─ MAC #1 → RTL8211FD #1
│  ├─ MAC #2 → RTL8211FD #2
│  └─ RJ45 连接器与变压器
│
├─ Sheet 4: PTP 时间戳
│  ├─ 时钟振荡器
│  ├─ 计数器 / 时间戳逻辑
│  └─ GPIO 时钟输出
│
├─ Sheet 5: 扩展接口
│  ├─ USB 3.0 Hub (可选)
│  ├─ GPIO 扩展
│  └─ UART 调试
│
└─ Sheet 6: 监控与保护
   ├─ 温度传感器
   ├─ 电源监控 (可选)
   └─ EMI 过滤
```

---

## W2-W4 设计计划

### W2 (Jan 27 - Feb 2): 原理图初稿与电源设计

**目标**: 完成原理图 50%，重点电源系统

| 任务 | 预期 | 交付物 |
|------|------|--------|
| KiCad 项目初始化 | W2-1 | 项目框架 |
| 电源系统原理图 | W2-2 | Sheet 1 完成 |
| 处理器接口 | W2-3 | Sheet 2 框架 |
| 网络接口初稿 | W2-4 | Sheet 3 (50%) |

**交付**:
- [ ] `hardware/kicad/zone-controller-sch.kicad_sch` (原理图初稿 50%)
- [ ] 电源设计计算文档
- [ ] 原理图审查清单

### W3 (Feb 3-9): 原理图完成与初步布局

**目标**: 原理图 100%，PCB 初步布局

| 任务 | 预期 | 交付物 |
|------|------|--------|
| 网络接口完成 | W3-1 | Sheet 3 完成 |
| PTP 时间戳设计 | W3-2 | Sheet 4 完成 |
| 扩展接口 | W3-3 | Sheet 5 完成 |
| 监控与保护 | W3-4 | Sheet 6 完成 |

**交付**:
- [ ] 完整原理图 (6 页)
- [ ] 初步 PCB 布局框架
- [ ] 设计检查清单 (DFM)

### W4 (Feb 10-16): PCB 设计与制造准备

**目标**: PCB 完整设计，生成制造文件

| 任务 | 预期 | 交付物 |
|------|------|--------|
| 元器件放置 | W4-1 | 放置完成 |
| 信号布线 | W4-2 | 布线 80% |
| 电源/GND 铺铜 | W4-3 | 完成 |
| 生成 Gerber 文件 | W4-4 | PCB 制造文件 |

**交付**:
- [ ] PCB 设计完成 (4 层, 150×100mm)
- [ ] Gerber + Drill 文件
- [ ] BOM 与装配文件
- [ ] PCB 制造指令

---

**制定日期**: 2026-01-20  
**当前阶段**: Stage 2 W2  
**目标完成**: 2026-02-16  
**设计评审**: 2026-01-27 (15:00)
