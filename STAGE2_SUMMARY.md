# OpenNeuro 阶段二计划 - 完整交付物总结

**生成日期**: 2026 年 1 月 20 日  
**计划周期**: 9 个月 (Q2-Q4 2026)  
**版本**: v1.0

---

## 📦 本次交付内容

本次规划阶段已交付**三份详细的技术设计文档**，为 OpenNeuro 的下一代实时通信系统奠定基础。

### 📄 交付文件清单

1. **STAGE2_PLAN.md** (项目总体计划)
   - 📊 9 个月详细时间表
   - 👥 团队构成和分工建议
   - 💰 预算估算与 ROI 分析
   - 🎯 三个主要子项目分解:
     - 项目 2A: PTP 时间同步 (3-4 个月)
     - 项目 2B: TSN 网络调度 (4-6 个月)
     - 项目 2C: 参考硬件设计 (6-9 个月)

2. **docs/PTP_DESIGN.md** (时间同步详细设计)
   - 🔄 Master-Slave 同步机制
   - 💾 完整的数据结构定义
   - 🛠️ 硬件适配层 (STM32H7, RK3588, ESP32)
   - 📡 IEEE 1588 网络消息格式
   - 🧪 测试计划和验收标准
   - **代码框架**: 500+ 行 C 伪代码示例

3. **docs/TSN_DESIGN.md** (网络调度详细设计)
   - 🎨 时间感知门禁调度算法
   - 📐 队列和优先级映射
   - 🔧 Linux tc/taprio 配置详解
   - 💾 完整的流配置数据结构
   - 🛠️ 硬件兼容性矩阵
   - 📊 性能分析和预测模型
   - **配置示例**: YAML 格式完整示例

---

## 🎯 关键设计成果

### PTP 时间同步 (Project 2A)

#### 设计目标
```
同步精度: <10 µs (P99) ✓
稳定性: >99.9% ✓
支持节点: >16 个 ✓
代码大小: <50 KB ✓
```

#### 核心技术方案

**1. Master-Slave 架构**
```
Master (RK3588/PC)
    │ SYNC (100ms)
    ├─→ Slave 1 (STM32H7)
    ├─→ Slave 2 (RK3588)
    └─→ Slave N (Jetson)
    
精度递进:
├─ 初级同步: ±100 µs (SYNC/FOLLOW_UP)
├─ 精细同步: ±10 µs (DELAY_REQ/DELAY_RESP)
└─ 锁定状态: ±1 µs (PI 伺服控制)
```

**2. PI 伺服控制器**
- 比例增益 (Kp): 控制快速响应
- 积分增益 (Ki): 消除持续偏差
- 频率微调 (PPM): 长期稳定性
- 步进调整 (ns): 快速初始锁定

**3. 硬件时间戳**

| 平台 | 实现方式 | 精度 | 备注 |
|------|---------|------|------|
| STM32H7 | 以太网 MAC 硬件单元 | ±50ns | 原生支持 |
| RK3588 | Linux CLOCK_TAI | ±100ns | kernel 驱动 |
| ESP32 | FreeRTOS 定时器 | ±1µs | 软件实现 |
| Jetson | NVIDIA PTP | ±100ns | 硬件支持 |

#### 实现阶段

```
Phase 2A-1 (4 周): 协议栈开发
  ├─ libptp_master.c (发送 SYNC/FOLLOW_UP)
  ├─ libptp_slave.c (接收并同步)
  ├─ ptp_servo.c (PI 控制)
  └─ 状态机实现

Phase 2A-2 (3 周): 硬件集成
  ├─ stm32h7_ptp_hw.c
  ├─ rk3588_ptp_hw.c
  └─ esp32_ptp_hw.c

Phase 2A-3 (2 周): 验证和优化
  ├─ 单节点精度测试
  ├─ 多节点同步验证
  ├─ 24 小时稳定性测试
  └─ 性能报告生成
```

#### 成功指标
- ✅ Slave 与 Master 偏差 <10µs (P99)
- ✅ 锁定时间 <30 秒
- ✅ 无时间倒流现象
- ✅ 长期漂移 <1µs/分钟

---

### TSN 网络调度 (Project 2B)

#### 设计目标
```
最大延迟: <1 ms (P99) ✓
丢包率: 0 (<0.01%) ✓
支持流数: >100 ✓
交换机支持: 3+ 型号 ✓
```

#### 核心技术方案

**1. 时间感知门禁调度 (802.1Qbv)**

```
周期: 1 ms (可配置)

0µs      100µs        500µs        1000µs
├─门 0──┤ (关键)      │            │
│ 开    关闭          │            │
│       ├─门 1────┤ (实时)       │
│       │ 开      关闭            │
│       │       ├─门 2────────┤ (普通)
│       │       │ 开         关闭
└───────┴───────┴────────────┴────→ 时间

优先级分配:
  0-2: 关键任务 (电机控制)
  3-5: 实时任务 (传感器采样)
  6-7: 普通流量 (视频, 日志)
```

**2. Linux tc/taprio 实现**

```bash
tc qdisc replace dev eth0 root taprio \
  num_tc 8 \
  map 0 1 2 3 4 5 6 7 \
  queues 1@0 1@1 1@2 1@3 1@4 1@5 1@6 1@7 \
  base-time 1705715200000000000 \
  sched-entry S 0x00FF 100000 \  # 100µs, 队列 0-7
  sched-entry S 0xFF00 900000 \  # 900µs, 队列 8+
  clockid CLOCK_TAI
```

**3. 流过滤和整形**

```
应用                端口        优先级    最大延迟
──────────────────────────────────────────────
电机控制            5000        0         <500µs
传感器 1-8          5001-5008   2         <5000µs
视频编码            5010-5020   6         <33000µs
```

#### 实现阶段

```
Phase 2B-1 (3 周): API 设计
  ├─ tsn_api.h (C 语言 API)
  ├─ tsn_stream_create()
  ├─ tsn_set_gate_schedule()
  └─ tsn_stream_send/receive()

Phase 2B-2 (3 周): Linux 驱动
  ├─ taprio qdisc 配置
  ├─ tc flower 流分类
  ├─ 令桶整形器
  └─ 统计和监控

Phase 2B-3 (2 周): 硬件验证
  ├─ 交换机兼容性测试
  ├─ 性能基准测试
  ├─ 故障模式注入
  └─ 文档完善
```

#### 成功指标
- ✅ 电机控制延迟 <500µs
- ✅ 视频传输无丢包
- ✅ 多流隔离 >99.9%
- ✅ 支持 Marvell/Realtek/Broadcom 交换机

---

### 参考硬件设计 (Project 2C)

#### 设计目标
```
MCU: RK3588 或 i.MX 8M Plus
网络: 2x Gigabit Ethernet (冗余)
I/O: 4x SPI, 8x I2C, 16x GPIO
功率: 48V PoE 供电
尺寸: 10cm x 10cm (紧凑)
成本: <$200 BOM
```

#### 设计成果

**1. 原理图设计** (4 周)
```
KiCad 项目结构:
├─ OpenNeuro-ZoneController.sch
├─ 子图纸:
│  ├─ power.sch (48V → 12V → 3.3V)
│  ├─ ethernet.sch (2x PHY + 磁盘)
│  ├─ rk3588_core.sch (核心 SoM)
│  ├─ io_expansion.sch (SPI/I2C/GPIO)
│  └─ debugging.sch (UART/JTAG)
├─ BOM.csv (完整物料清单)
└─ power_analysis.md
```

**2. PCB 布局设计** (6 周)
```
关键考虑:
├─ DDR3/DDR4 信号完整性
├─ Ethernet 差分对走线
├─ 电源分配网络 (PDN)
├─ 散热设计 (RK3588 TDP ~5W)
├─ EMI/EMC 屏蔽
└─ 制造工艺检查 (DFM)
```

**3. 标准化接口: Neuro-Link 连接器**
```
40-pin 0.1" 梯形连接器

引脚分配:
├─ 电源 (Pin 1-4): +5V, GND, +3.3V, GND
├─ 高速 (Pin 5-10): Ethernet LVDS (可选)
├─ 控制 (Pin 11-30): 
│  ├─ PWM Out x8 (驱动电机)
│  └─ GPIO In x8 (读取传感器)
├─ 调试 (Pin 31-38): UART, JTAG
└─ 预留 (Pin 39-40): 向后兼容
```

#### 第一版时间表

```
阶段 2C-1 (4 周): 原理图设计
阶段 2C-2 (6 周): PCB 布局和 DFM 检查
阶段 2C-3 (6 周): 原型制造和验证
  ├─ 小批量 (10 块)
  ├─ 功能测试 (通电, 网络, I/O)
  ├─ 可靠性测试 (温度, 湿度, 振动)
  └─ 文档和制造规范
```

---

## 📅 整体时间表

### Quarter 分解

**Q2 2026 (Apr - Jun)**
```
├─ PTP 协议栈完全实现 (M2.1, M2.2)
├─ TSN API 设计和初始实现
├─ 硬件设计阶段 1 (原理图)
└─ 里程碑: PTP Beta 版本可用
```

**Q3 2026 (Jul - Sep)**
```
├─ PTP 性能验证和优化
├─ TSN 完整实现和测试 (M2.3, M2.4)
├─ 硬件设计阶段 2-3 进行中
└─ 里程碑: TSN 完全可用，硬件原型完成
```

**Q4 2026 (Oct - Dec)**
```
├─ 全系统集成和压力测试
├─ 硬件原型验证
├─ 文档完善和翻译
├─ 社区发布准备
└─ 里程碑: v0.2.0 发布 (完整阶段二)
```

---

## 💼 组织和资源

### 推荐团队结构

```
工作组总经理
    │
    ├─ PTP 工作组负责人
    │  ├─ 协议开发工程师 x1
    │  ├─ 硬件集成工程师 x1
    │  └─ 测试工程师 (兼职 0.5)
    │
    ├─ TSN 工作组负责人
    │  ├─ 网络工程师 x1.5
    │  ├─ Linux 驱动工程师 x1
    │  └─ 测试工程师 (兼职 0.5)
    │
    ├─ 硬件设计工作组负责人
    │  ├─ 硬件设计工程师 x1
    │  └─ PCB 设计工程师 (外包)
    │
    └─ 项目管理和技术文档
       └─ PM/文档工程师 x0.5
```

### 总投入

```
人力成本:        ¥250,000 (4.5 FTE × 9 个月)
硬件成本:        ¥ 15,000 (PCB, 测试仪器)
工具和软件:      ¥  5,000 (许可证等)
文档和培训:      ¥ 10,000 (教学视频等)
杂项:            ¥  5,000
──────────────────────────
总计:            ¥ 285,000
```

### ROI 分析

```
收益来源:
1. 商业授权费 (50-100 家企业)          €10-20K
2. 技术咨询服务                        €5-15K/项目
3. 参考硬件销售                        €50-100K
4. 标准合作 (IEEE, TSN Group)          €5-10K

预期 18 个月内投资回本
长期价值: 建立工业级实时通信新标准
```

---

## 🎓 技术难点和解决方案

### PTP 方面

| 难点 | 影响 | 解决方案 |
|------|------|---------|
| 硬件时间戳精度 | 同步精度上限 | 选择支持纳秒精度的硬件 |
| 网络抖动 | 同步不稳定 | 使用滤波器和 PI 伺服 |
| MCU 多样性 | 集成复杂 | 分层硬件抽象 |
| 时间倒流 | 应用崩溃 | 虚拟时钟机制 |

### TSN 方面

| 难点 | 影响 | 解决方案 |
|------|------|---------|
| 交换机支持不足 | 功能受限 | 多厂商测试, 提供兼容列表 |
| 门禁调度复杂 | 配置困难 | 自动化工具和模板 |
| 动态流加入 | 性能不可预测 | 准入控制和带宽预留 |
| 故障诊断 | 运维困难 | 详细的监控和日志 |

---

## 🚀 启动准备 (Next 2 Weeks)

### 第 1 周: 筹备

- [ ] 阅读本规划文档，理解整体愿景
- [ ] 复习 PTP 设计文档和 TSN 设计文档
- [ ] 建立项目管理系统 (GitHub Projects, Wiki)
- [ ] 准备开发环境和工具链
- [ ] 确认硬件采购清单

### 第 2 周: 启动

- [ ] 举办项目启动大会
- [ ] 分配工作任务和负责人
- [ ] 建立开发规范和流程
- [ ] 创建 PTP 协议栈初始代码框架
- [ ] 社区公告和招募

---

## ✅ 验收标准

### 阶段二完成条件

**技术交付**:
- ✅ PTP 协议栈代码 (>2000 LOC)
- ✅ 支持 4+ 硬件平台
- ✅ 同步精度 <10µs (经过验证)
- ✅ TSN 调度完整实现
- ✅ 支持 100+ 流
- ✅ 参考硬件设计完成
- ✅ 自动化测试覆盖率 >90%

**文档交付**:
- ✅ API 文档 (C/Python)
- ✅ 集成指南 (5 个案例)
- ✅ 性能报告
- ✅ 故障排除指南
- ✅ 硬件组装手册

**社区建设**:
- ✅ 3-5 个早期采用者
- ✅ GitHub 讨论社区
- ✅ 月度技术分享
- ✅ 贡献者指南

---

## 📚 附录：参考资源

### 国际标准

- IEEE 1588-2019: Precision Time Protocol
- IEEE 802.1AS: Timing and Synchronization
- IEEE 802.1Qbv: Time-Aware Shaper
- IEEE 802.1Qci: Per-Stream Filtering

### 开源项目参考

- linuxptp: https://github.com/richardcochran/linuxptp
- Linux kernel tc/taprio: https://kernel.org
- OVS (Open vSwitch): https://www.openvswitch.org/
- NXP TSN Tools: https://github.com/nxp-qoriq/

### 学术资源

- "Real-Time Ethernet" by Hermann Kopetz
- IEEE Std 802.1 Working Group
- TSN Consortium: https://tsn.readthedocs.io/

---

## 📞 联系和支持

**项目主页**: https://github.com/OpenRobotics/OpenNeuro  
**讨论区**: https://github.com/OpenRobotics/OpenNeuro/discussions  
**技术问题**: https://github.com/OpenRobotics/OpenNeuro/issues  
**邮件列表**: openneuro-dev@lists.example.com

---

## 📋 下一步行动

### 立即行动 (本周)

1. [ ] 完整阅读本规划文档
2. [ ] 评审 PTP 和 TSN 技术设计
3. [ ] 确认资源和人力可用性
4. [ ] 建立项目管理框架

### 短期行动 (1-2 周内)

1. [ ] 组织项目启动会议
2. [ ] 分配工作任务
3. [ ] 建立开发环境
4. [ ] 创建初始代码框架

### 中期目标 (下一个月)

1. [ ] PTP 协议栈 Alpha 版本
2. [ ] TSN API 设计完成
3. [ ] 社区公告发布
4. [ ] 硬件采购完成

---

**阶段二规划文档完成！** 🎉

所有文件已保存在 `d:\VScode专用\OpenNeuro\` 目录：
- `STAGE2_PLAN.md` - 项目总体规划
- `docs/PTP_DESIGN.md` - PTP 技术设计
- `docs/TSN_DESIGN.md` - TSN 技术设计
- `STAGE2_SUMMARY.md` - 本总结文档

现在已为期 9 个月的深度工程工作做好充分准备。

**下一步**: 启动 PTP 协议栈开发 (Project 2A, Phase 1)

