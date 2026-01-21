# M2.1 Alpha 验收标准与交付清单

**版本**: v1.0  
**日期**: 2026-01-20  
**发布日期**: 2026-02-16 (目标)  
**发布版本**: v0.2.0-alpha.1  
**发布类型**: Internal Alpha Release

---

## 📋 目录

1. [发布概述](#发布概述)
2. [代码质量标准](#代码质量标准)
3. [功能完成度标准](#功能完成度标准)
4. [性能指标标准](#性能指标标准)
5. [硬件设计标准](#硬件设计标准)
6. [交付物清单](#交付物清单)
7. [发布流程](#发布流程)

---

## 发布概述

### M2.1 定位

```
发布名称: OpenNeuro M2.1 Alpha
版本号: v0.2.0-alpha.1
发布日期: 2026-02-16
发布方式: GitHub Release (内部 + 早期用户测试)
目标受众: 内部团队, 技术合作伙伴, 开源社区初期贡献者

发布承诺:
├─ ✅ PTP Master 协议栈完整实现
├─ ✅ TSN 调度器算法完整实现
├─ ✅ 硬件参考设计 (原理图 + PCB)
├─ ✅ 基础文档与使用指南
└─ ⚠️ Beta 功能: Slave 实现, 多 Master 支持 (计划 M2.2)
```

### M2.1 不包含的功能

```
❌ PTP Slave 模式 (计划 M2.2)
❌ 高可用性 (Master failover) (计划 M2.3)
❌ Web 管理界面 (计划 M3.0)
❌ 生产级硬件认证 (计划 M2.4 GA)
❌ 商业许可 (仅开源)
```

---

## 代码质量标准

### 代码行数与覆盖率

```
模块                  目标 LOC    实际 LOC    覆盖率    状态
───────────────────────────────────────────────────────
PTP Stack
  ├─ messages.c       150        [实际]     ≥80%     [待填]
  ├─ bmc.c            100        [实际]     ≥80%     [待填]
  ├─ clock.c          80         [实际]     ≥80%     [待填]
  ├─ servo.c          80         [实际]     ≥80%     [待填]
  └─ api.c            60         [实际]     ≥80%     [待填]
  小计                550        550+       80%+     ✅ 目标

TSN Scheduler
  ├─ queue.c          120        [实际]     ≥80%     [待填]
  ├─ scheduler.c      150        [实际]     ≥80%     [待填]
  ├─ bandwidth.c      80         [实际]     ≥80%     [待填]
  ├─ linux_if.c       80         [实际]     ≥80%     [待填]
  └─ api.c            60         [实际]     ≥80%     [待填]
  小计                570        570+       80%+     ✅ 目标

总计                  1120       1120+      80%+
```

### 编译与静态分析

```
✅ 编译无错误
   └─ 所有 warnings 已解决

✅ 编译无警告 (-Wall -Wextra -Werror)
   └─ 包括 GCC 8.2+, Clang 10.0+

✅ 代码格式化
   └─ 符合 LLVM clang-format 风格

✅ 静态分析 (Clang Static Analyzer)
   └─ 0 个警告, 0 个潜在 bug

✅ 内存检查 (Valgrind)
   ├─ 0 个内存泄漏 (24h 运行)
   └─ 0 个 use-after-free

✅ 地址检查 (AddressSanitizer)
   ├─ 0 个堆溢出
   ├─ 0 个栈溢出
   └─ 0 个 use-after-free
```

### 代码审查标准

```
每个 Commit/PR 需要满足:

☑️ 至少 1 名资深工程师 Review 并批准
☑️ CI/CD 全部通过 (编译 + 测试)
☑️ 代码覆盖率 ≥ 80% (新增代码)
☑️ 性能无回退 (基准测试 ±5% 范围内)
☑️ 提交信息清晰 (格式: [Module] Brief description)
☑️ 没有代码注释中的 TODO 或 FIXME (除非有 issue 关联)

PR Template 示例:

Title: [Module] Feature/Fix description

## Description
What changes did you make?

## Related Issue
Closes #123

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Performance improvement
- [ ] Documentation

## Testing
How to test this change?

## Checklist
- [ ] Code compiles without warnings
- [ ] Tests pass locally
- [ ] Test coverage ≥80%
- [ ] Valgrind: 0 leaks
- [ ] No performance regression
```

---

## 功能完成度标准

### PTP Master 功能清单

```
☑️ 基础功能

【ANNOUNCE 消息】
  ☑️ 生成 (1Hz 周期)
  ☑️ 发送到多播地址 224.0.1.129:319
  ☑️ 包含正确的源时钟 ID 和优先级
  ☑️ 支持可配置的宣告间隔

【SYNC 消息】
  ☑️ 生成 (16s 周期或可配置)
  ☑️ 硬件时间戳捕获 (±10ns)
  ☑️ 发送到多播地址
  ☑️ 每个 SYNC 关联一个 FOLLOW_UP

【FOLLOW_UP 消息】
  ☑️ 发送精确时间戳 (t2)
  ☑️ 时钟标识
  ☑️ 无错误字段

【DELAY_REQ/RESP】
  ☑️ 接收来自 Slave 的 DELAY_REQ
  ☑️ 捕获接收时间戳 (t3, 硬件)
  ☑️ 发送 DELAY_RESP (包含 t3)
  ☑️ 正确的消息关联

☑️ 状态机与管理

【Master 状态机】
  ☑️ INITIALIZING → LISTENING → MASTER
  ☑️ 状态转换逻辑正确
  ☑️ 故障时自动降级 (如需要)
  ☑️ 持久化 BEST (可选)

【时钟管理】
  ☑️ 64-bit 秒 + 32-bit 纳秒精度
  ☑️ NTP/PTP 时间源支持 (可选)
  ☑️ 时钟频率测量
  ☑️ UTC 偏移管理

☑️ 高级特性

【BMC 算法】
  ☑️ 多 Master 比较
  ☑️ BEST_MASTER_CLOCK 选择
  ☑️ 优先级排序正确
  ☑️ 单元测试覆盖 ≥80%

【PI 伺服算法】
  ☑️ 频率调整 (Kp, Ki 参数可调)
  ☑️ 相位调整
  ☑️ 抗振荡能力
  ☑️ 性能: <20µs 稳态误差

【硬件时间戳】
  ☑️ Rx 端时间戳精度 ±10ns
  ☑️ Tx 端时间戳精度 ±10ns
  ☑️ 时间戳与消息的关联正确
  ☑️ 支持多级物理接口

验收标准:
  ✅ 所有 ☑️ 项目 == 完成
  ✅ 单元测试覆盖率 ≥80%
  ✅ 集成测试通过 (8 个 Slave)
  ✅ 24h 运行无阻塞
  ✅ 性能达成: <20µs
```

### TSN Scheduler 功能清单

```
☑️ 基础功能

【流量分类】
  ☑️ 5-tuple 规则引擎 (源IP, 目标IP, 协议, 源端口, 目标端口)
  ☑️ VLAN tag 识别 (可选)
  ☑️ IP ToS/DSCP 字段解析
  ☑️ 支持 ≥256 条规则
  ☑️ 动态规则更新 (无需重启)

【优先级映射】
  ☑️ 8 级优先级队列 (0-7, 7 最高)
  ☑️ RT 流 (Real-time): 优先级 6-7
  ☑️ ST 流 (Scheduled): 优先级 4-5
  ☑️ CT 流 (Control): 优先级 2-3
  ☑️ BE 流 (Best-effort): 优先级 0-1
  ☑️ 优先级可配置

【带宽控制】
  ☑️ 入场控制 (Admission Control)
  ☑️ 流的带宽预留 (Mbps 级精度)
  ☑️ 总带宽限制 (可配置)
  ☑️ 流数量限制 (≥512 流)
  ☑️ 实时可用带宽查询

【门控调度】
  ☑️ 1ms 周期时间轴
  ☑️ 每周期 4 个时间槽:
     - RT: 0-100µs (门开放)
     - ST: 100-400µs (门开放)
     - CT: 400-900µs (门开放)
     - BE: 900-1000µs (门开放)
  ☑️ 门控状态机正确
  ☑️ 无丢包 (除非带宽不足)

☑️ 高级特性

【Linux 集成】
  ☑️ tc qdisc add (taprio 队列规则)
  ☑️ tc filter add (流分类规则)
  ☑️ Linux 驱动支持 (RTL8211FD PHY)
  ☑️ ethtool 接口查询配置
  ☑️ sysfs 实时监控 (流量统计)

【API 接口】
  ☑️ C API (ptp_stack, tsn_scheduler)
  ☑️ Python API (通过 ctypes 或 swig)
  ☑️ 完整的错误处理与返回码
  ☑️ 文档齐全 (Doxygen)

【可观测性】
  ☑️ 流量统计 (入口/出口报文数、字节数)
  ☑️ 丢包计数
  ☑️ 延迟统计 (min/max/avg)
  ☑️ 队列深度监控
  ☑️ CPU 使用率监控

验收标准:
  ✅ 所有 ☑️ 项目 == 完成
  ✅ 单元测试覆盖率 ≥80%
  ✅ 集成测试通过 (10+ 流量组合)
  ✅ 24h 运行无阻塞
  ✅ 性能达成: <100µs 延迟, 0 丢包
  ✅ Linux 集成验证成功
```

---

## 性能指标标准

### PTP 性能基准

```
指标                  目标        实测        状态
─────────────────────────────────────────
时间同步精度           <20µs       [实际]      ☑️
同步收敛时间           <10s        [实际]      ☑️
消息处理延迟           <10µs       [实际]      ☑️
时钟稳定性 (Allan)     <1e-6       [实际]      ☑️
最大误差               <50µs       [实际]      ☑️
CPU 占用率             <5%         [实际]      ☑️
内存占用               <1MB        [实际]      ☑️
Slave 数量             ≥8          [实际]      ☑️

测试场景:
├─ 单 Master + 8 Slave (内网)
├─ 运行时间: 24 小时
├─ 网络条件: 有丢包 (1-5%), 有延迟变化
└─ CPU 负载: 并行跑其他工作负载

验收条件:
  ✅ 所有指标达成或超期望
  ✅ 无超时、无崩溃、无阻塞
  ✅ 数据波动正常 (峰值 <3x 均值)
```

### TSN 性能基准

```
指标                  目标        实测        状态
─────────────────────────────────────────
RT 流延迟             <100µs      [实际]      ☑️
ST 流延迟             <500µs      [实际]      ☑️
CT 流延迟             <1ms        [实际]      ☑️
丢包率 (RT)           0%          [实际]      ☑️
丢包率 (ST)           0%          [实际]      ☑️
吞吐量                95%+        [实际]      ☑️
CPU 占用              <10%        [实际]      ☑️
流数量支持            256+        [实际]      ☑️
规则数支持            512+        [实际]      ☑️

测试场景:
├─ 混合流量 (RT 4, ST 2, CT 2, BE N)
├─ 流量强度: 80% 链路利用率
├─ 时间: 24 小时
├─ 网络: 局域网 (Gigabit Ethernet)
└─ 硬件: RK3588 Zone Controller

验收条件:
  ✅ RT 流 0 丢包
  ✅ ST 流 0 丢包 (优先级内)
  ✅ 延迟符合目标
  ✅ CPU <10% 处于 1Gbps 链路
  ✅ 热稳定 (<50°C)
```

### 硬件成本基准

```
成本分析                  目标      实测    变化
────────────────────────────────────────────
单件 DIY 成本            ¥361      ¥[X]   [±5%]
单件组装成本             ¥441      ¥[X]   [±5%]
100+ 批量成本            ¥310      ¥[X]   [±5%]

成本控制:
  ✅ 单件成本 ≤ ¥380 (5% 容差)
  ✅ 100+ 批量 ≤ ¥320
  ✅ 无超期望成本 (±10%)

如果超支:
  → 审视器件采购(是否有更便宜替代)
  → 评估设计简化空间
  → 采购量达到目标时成本改善
```

---

## 硬件设计标准

### 原理图设计标准

```
☑️ 设计完整性

【电源系统】
  ☑️ 输入保护 (熔断器 + TVS 二极管)
  ☑️ MP2107 #1: 12V → 3.3V @ 5A (RK3588 核心)
  ☑️ MP2107 #2: 12V → 5V @ 3A (USB + 外设)
  ☑️ 滤波电容: 100µF 输入, 22µF 输出 (根据数据表)
  ☑️ 过流保护 (自恢复熔断器或电流限制)

【处理器模块】
  ☑️ Orange Pi 5 Plus SoM 集成
  ☑️ 100-pin SO-DIMM 连接器
  ☑️ DDR5 + eMMC 信号完整
  ☑️ RTC 备用电池支持

【网络接口】
  ☑️ 2× RTL8211FD PHY
  ☑️ RGMII 信号完整
  ☑️ RJ45 连接器 + 变压器
  ☑️ 晶体振荡器 (125MHz)
  ☑️ LED 指示灯 (Link, Activity, Speed)

【PTP 硬件时间戳】
  ☑️ 时间戳采集电路
  ☑️ 1PPS 输出到 GPIO
  ☑️ 时钟计数器 (64-bit)
  ☑️ 温度补偿电路 (可选)

【扩展接口】
  ☑️ USB 3.0 Hub (可选)
  ☑️ 40-pin GPIO (树莓派兼容)
  ☑️ UART 调试接口
  ☑️ SPI/I2C (可选)

【保护与监控】
  ☑️ EMI 滤波器
  ☑️ 板层接地设计 (GND 平面)
  ☑️ 热处理 (铝壳 + 导热膏)
  ☑️ 温度传感器 (可选)

验收标准:
  ✅ 原理图 100% 完成 (6 页)
  ✅ 所有 ☑️ 项目实现
  ✅ DFM 检查通过
  ✅ 信号完整性验证
  ✅ 电源完整性验证
```

### PCB 设计标准

```
☑️ PCB 规格

【板层设计】
  ☑️ 4 层 PCB:
     - Layer 1: Signal (Tx, Rx, 时序敏感)
     - Layer 2: GND (Ground Plane)
     - Layer 3: VCC (Power Plane)
     - Layer 4: Signal (底层, 一般信号)
  ☑️ 板厚: 1.6mm
  ☑️ 阻抗控制: 100Ω 差分 (以太网)

【设计规则】
  ☑️ 最小线宽: 0.25mm
  ☑️ 最小间距: 0.25mm
  ☑️ Via 孔径: 0.3mm
  ☑️ 过孔位置: 器件下方或引脚旁 (最近原则)
  ☑️ 内层间距: ≥0.3mm

【高速信号完整性】
  ☑️ 以太网 RGMII: 差分阻抗 100Ω, 等长设计
  ☑️ DDR 信号: 等长 ±5mm, 阻抗 50Ω
  ☑️ 时钟信号: 短路径, 避免平行走线
  ☑️ 返回路径: 连续 GND 平面

【电源完整性】
  ☑️ 电源层覆盖率 ≥95%
  ☑️ 电容分布: 靠近 IC 供电脚 (≤10mm)
  ☑️ 通孔分布: 多个分布式过孔连接电源/地
  ☑️ 无孤岛: 所有电源和地连通

【热管理】
  ☑️ RK3588  下方 PCB 铜箱 (增加散热面积)
  ☑️ 散热硅胶垫 (厚 1-2mm)
  ☑️ 铝壳接触 PCB 散热区
  ☑️ 环境: 开放空气, 气流存在

【制造工艺】
  ☑️ 所有焊盘 NSMD (Non-solder-mask-defined)
  ☑️ 板边到焊盘距离 ≥0.5mm
  ☑️ 丝印: 清晰可读, 不覆盖焊盘
  ☑️ 测试点: 关键信号预留测试点

验收标准:
  ✅ PCB 设计 100% 完成
  ✅ DFM 检查通过 (制造商认可)
  ✅ Gerber 文件生成并验证
  ✅ 成本符合目标 (≤¥380/单件)
  ✅ 制造周期 ≤2 周 (含交付)
  ✅ 首板可以直接使用 (无 ECO 修改)
```

---

## 交付物清单

### 代码与项目文件

```
📦 ptp-stack/
├─ src/
│  ├─ messages.c/h      (150 LOC, ANNOUNCE/SYNC/DELAY)
│  ├─ bmc.c/h           (100 LOC, BMC 算法)
│  ├─ clock.c/h         (80 LOC, 时钟管理)
│  ├─ servo.c/h         (80 LOC, PI 伺服)
│  ├─ api.c/h           (60 LOC, 公开 API)
│  └─ ...
├─ test/
│  ├─ test_messages.c   (40 LOC, 单元测试)
│  ├─ test_bmc.c        (30 LOC)
│  ├─ test_clock.c      (25 LOC)
│  └─ ...
├─ CMakeLists.txt       (编译配置)
├─ README.md            (项目说明)
└─ docs/
   └─ API_REFERENCE.md  (API 文档)

✅ 编译验证: gcc/clang -Wall -Wextra -Werror 通过
✅ 单元测试: 550+ LOC, 80%+ 覆盖率
✅ 代码审查: 0 个 TODO, 0 个 FIXME

📦 tsn-scheduler/
├─ src/
│  ├─ queue.c/h         (120 LOC, 优先级队列)
│  ├─ scheduler.c/h     (150 LOC, 门控调度)
│  ├─ bandwidth.c/h     (80 LOC, 带宽控制)
│  ├─ linux_if.c/h      (80 LOC, Linux 集成)
│  ├─ api.c/h           (60 LOC, 公开 API)
│  └─ ...
├─ test/
│  ├─ test_queue.c      (30 LOC)
│  ├─ test_scheduler.c  (40 LOC)
│  ├─ test_integration.c (35 LOC, 集成测试)
│  └─ ...
├─ CMakeLists.txt
├─ README.md
└─ docs/
   └─ API_REFERENCE.md

✅ 编译验证: 570+ LOC, 80%+ 覆盖率
✅ Linux 集成: taprio 接口验证
✅ 代码审查: 0 个 TODO, 0 个 FIXME

📦 zone-controller/ (硬件设计)
├─ hardware/
│  ├─ kicad/
│  │  ├─ zone-controller.pro        (KiCad 项目)
│  │  ├─ zone-controller-sch.kicad_sch (原理图)
│  │  ├─ zone-controller-pcb.kicad_pcb (PCB 布局)
│  │  ├─ sym/                       (符号库)
│  │  └─ fp/                        (封装库)
│  ├─ gerber/
│  │  ├─ zone-controller-F.Cu.gbr   (顶层)
│  │  ├─ zone-controller-B.Cu.gbr   (底层)
│  │  ├─ zone-controller-F.Mask.gbr (阻焊)
│  │  ├─ zone-controller-B.Mask.gbr
│  │  ├─ zone-controller-F.SilkS.gbr (丝印)
│  │  ├─ zone-controller-B.SilkS.gbr
│  │  ├─ zone-controller-Edge.Cut.gbr (板边界)
│  │  ├─ zone-controller-PTH.txt    (通孔表)
│  │  └─ zone-controller.drl        (钻孔表)
│  └─ docs/
│     ├─ DESIGN_SPECIFICATION.md    (设计规范)
│     ├─ BOM_DETAILED.md            (物料清单 + 成本)
│     ├─ SCHEMATIC_DESIGN_GUIDE.md  (原理图指南)
│     ├─ PCB_DESIGN_GUIDE.md        (PCB 指南, 可选)
│     └─ Assembly_Instructions.pdf  (组装说明)

✅ 原理图: 100% 完成, DFM 检查通过
✅ PCB: 100% 完成, Gerber 文件生成
✅ 成本: ¥361 单件, ¥310 批量

📦 文档与参考资料
├─ docs/
│  ├─ PTP_ARCHITECTURE.md           (架构设计)
│  ├─ PTP_MASTER_IMPLEMENTATION.md  (实现指南)
│  ├─ TSN_REQUIREMENTS.md           (需求规范)
│  ├─ TSN_ALGORITHM_DESIGN.md       (算法设计)
│  ├─ HARDWARE_DESIGN_GUIDE.md      (硬件指南)
│  └─ API_REFERENCE.md             (完整 API 参考)
│
├─ guides/
│  ├─ QUICKSTART.md                 (5 分钟快速开始)
│  ├─ INSTALLATION.md               (安装说明)
│  ├─ CONFIGURATION.md              (配置指南)
│  ├─ TROUBLESHOOTING.md            (故障排除)
│  └─ PERFORMANCE_TUNING.md         (性能调优)
│
├─ examples/
│  ├─ simple_master.c               (简单 Master 示例)
│  ├─ simple_scheduler.c            (简单调度示例)
│  ├─ integration_demo.c            (集成示例)
│  └─ performance_test.py           (性能测试脚本)
│
└─ reports/
   ├─ PERFORMANCE_REPORT.md         (性能基准报告)
   ├─ TEST_COVERAGE_REPORT.html     (覆盖率报告)
   └─ FINAL_ASSESSMENT.md           (最终评估)
```

### 发布文档

```
📄 RELEASE_NOTES.md
├─ 版本: v0.2.0-alpha.1
├─ 发布日期: 2026-02-16
├─ 主要功能:
│  ├─ PTP Master 实现完成 (550 LOC, 80%+ 覆盖)
│  ├─ TSN Scheduler 实现完成 (570 LOC, 80%+ 覆盖)
│  └─ 硬件参考设计 (原理图 + PCB 完整)
│
├─ 已知限制:
│  ├─ PTP Slave 实现计划在 M2.2
│  ├─ 多 Master 故障转移计划在 M2.3
│  └─ 生产级认证计划在 M2.4 GA
│
├─ 性能指标:
│  ├─ PTP 延迟: <20µs (实测)
│  ├─ TSN 延迟: <100µs (实测)
│  └─ 成本: ¥361 单件
│
├─ 安装说明:
│  ├─ 依赖: GCC 8.2+, CMake 3.10+, Linux 5.0+
│  ├─ 编译: cmake .. && make
│  └─ 运行: ./examples/simple_master
│
├─ 贡献指南:
│  ├─ 代码风格: LLVM clang-format
│  ├─ 测试覆盖率: ≥80%
│  └─ 提交流程: GitHub PR
│
└─ 许可证: GPL-3.0 (开源)

📄 KNOWN_ISSUES.md
├─ 【P1 - 高优先】
│  ├─ Issue-001: Windows 编译不支持 (Linux only)
│  └─ Issue-002: 某些 RTL8211FD 版本 PTP 时间戳有延迟
│
├─ 【P2 - 中优先】
│  ├─ Issue-003: taprio 需要 Linux 5.0+ 内核
│  └─ Issue-004: 内存占用在低内存设备上过高
│
└─ 【P3 - 低优先】
   └─ Issue-005: 文档中某些代码示例需要更新

📄 UPGRADE_GUIDE.md
├─ 从 M2.0 → M2.1:
│  ├─ API 兼容性: 基本兼容 (新增接口无损)
│  ├─ 配置文件: 格式不变, 可直接升级
│  └─ 数据迁移: 无需迁移

📄 SUPPORT.md
├─ 问题报告: GitHub Issues
├─ 讨论: GitHub Discussions
├─ 实时支持: Discord Server (可选)
└─ 商业支持: 待定
```

---

## 发布流程

### 发布前清单 (2月14日)

```
【代码冻结】(2月12日)
  ☐ Master 分支冻结, 仅接受 bug fixes
  ☐ 所有 PR 已合并
  ☐ 代码覆盖率 ≥80% 验证
  ☐ 编译无错误无警告验证

【性能验证】(2月13日)
  ☐ PTP 性能基准 24h 测试通过
  ☐ TSN 性能基准 24h 测试通过
  ☐ 硬件集成测试通过
  ☐ 跨系统集成测试通过

【文档完成】(2月14日早)
  ☐ API 文档生成 (Doxygen)
  ☐ 使用指南完成
  ☐ 故障排除文档完成
  ☐ 性能报告生成

【发布材料准备】(2月14日)
  ☐ Release Notes 编写
  ☐ Known Issues 文档
  ☐ Upgrade Guide 编写
  ☐ 代码注释审查 (无 TODO/FIXME)

【质量确认】(2月14日下午)
  ☐ PM 最终验收
  ☐ 技术 Lead 签字确认
  ☐ 所有风险已评估
  ☐ 交付物清单已验证
```

### 发布步骤 (2月16日)

```
【代码标记】
  git tag -a v0.2.0-alpha.1 -m "M2.1 Alpha Release"
  git push origin v0.2.0-alpha.1

【GitHub Release 创建】
  标题: OpenNeuro M2.1 Alpha - v0.2.0-alpha.1
  
  内容: [参考 RELEASE_NOTES.md]
  
  附件上传:
    ├─ ptp-stack-v0.2.0-alpha.1.tar.gz
    ├─ tsn-scheduler-v0.2.0-alpha.1.tar.gz
    ├─ zone-controller-gerber-v0.2.0-alpha.1.zip
    └─ RELEASE_NOTES.md

【社区通知】
  ├─ GitHub Announcements
  ├─ Discord #announcements
  ├─ 邮件通知 (早期用户)
  └─ Twitter/LinkedIn (如有)

【文档部署】
  ├─ GitHub Pages 更新
  ├─ Wiki 更新
  └─ Read the Docs 发布 (如有)
```

### 发布后支持 (2月16日 onwards)

```
【反馈收集】(2周内)
  ├─ GitHub Issues 监控
  ├─ Discord 讨论监控
  ├─ 早期用户反馈收集
  └─ Bug 报告分类

【热补丁计划】
  ├─ P0 bugs: 24h 内修复 (v0.2.0-alpha.2)
  ├─ P1 bugs: 一周内修复
  └─ P2 bugs: 累积后在 M2.2 修复

【文档更新】
  ├─ 根据用户反馈更新文档
  ├─ FAQ 添加常见问题
  └─ 故障排除指南补充

【M2.2 计划】
  ├─ 启动日期: 2月17日
  ├─ 目标: PTP Slave 实现
  ├─ 预计发布: 5月 (M2.2 Beta)
  └─ 改进: 基于 M2.1 反馈
```

---

**制定日期**: 2026-01-20  
**目标发布日期**: 2026-02-16  
**发布版本**: v0.2.0-alpha.1  
**维护者**: Project Manager + Technical Leads
