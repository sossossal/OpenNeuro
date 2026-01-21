# OpenNeuro 阶段二 - 里程碑与交付清单

**文档版本**: v1.0  
**日期**: 2026 年 1 月 20 日  
**计划期限**: 2026 年 Q2 - Q4 (9 个月)  
**目标发布**: v0.2.0 (2026 年 12 月 15 日)

---

## 🎯 核心里程碑 (5 个)

### M2.1: PTP Alpha 版本 (2026-05-31)

**目标**: 完整的 PTP 协议栈实现，支持基本的主从同步

#### 交付物清单

- [x] **源代码**
  - `firmware/ptp/src/libptp_master.c` (~600 行)
  - `firmware/ptp/src/libptp_slave.c` (~700 行)
  - `firmware/ptp/src/ptp_clock.c` (~400 行)
  - `firmware/ptp/src/ptp_servo.c` (~300 行)
  - `firmware/ptp/src/ptp_messages.c` (~350 行)
  - `firmware/ptp/include/ptp.h` (公开 API)
  - `firmware/ptp/include/ptp_config.h` (可调参数)

- [x] **硬件适配**
  - `firmware/zenoh-pico-stm32/src/ptp_hw.c` (STM32 时间戳)
  - `firmware/zenoh-pico-esp32/src/ptp_hw.c` (ESP32 定时器)
  - `firmware/rk3588/src/ptp_hw.c` (Linux 系统时间)

- [x] **测试代码**
  - `tests/ptp_tests/test_ptp_basic.py` (基础功能)
  - `tests/ptp_tests/test_ptp_messages.py` (消息编解码)
  - `tests/ptp_tests/test_ptp_servo.py` (PI 控制器)

- [x] **文档**
  - `docs/PTP_DESIGN.md` (详细设计)
  - `firmware/ptp/README.md` (使用指南)
  - `docs/PTP_API_REFERENCE.md` (API 文档)

#### 验收标准

```
☐ Slave 能接收 Master 的 SYNC 消息
☐ 时间偏差计算正确 (偏差 <100µs)
☐ 代码测试覆盖率 >80%
☐ 编译无警告 (Werror 模式)
☐ 文档完整和正确
```

#### 评价指标

```
同步精度:   ±100 µs (初级目标)
稳定性:     >95% (初步)
CPU 占用:   <1%
内存使用:   <15 KB
```

---

### M2.2: PTP Beta 版本 (2026-06-30)

**目标**: 硬件集成完成，多节点同步验证通过

#### 交付物清单

- [x] **完整集成**
  - STM32H7 以太网硬件时间戳实现
  - RK3588 系统时间调整驱动
  - ESP32 FreeRTOS 定时器集成
  - 三个平台同步测试通过

- [x] **性能优化**
  - PI 伺服参数优化
  - 频率微调实现
  - 步进调整机制
  - 过滤算法 (Kalman filter 可选)

- [x] **测试报告**
  - `docs/PTP_PERFORMANCE_REPORT.md` (完整性能报告)
  - 单节点精度测试 (100 样本)
  - 多节点同步测试 (4 个 Slaves)
  - 24 小时稳定性测试

- [x] **文档补充**
  - `docs/PTP_TROUBLESHOOTING.md` (故障排查)
  - `docs/PTP_INTEGRATION_GUIDE.md` (集成指南)
  - 5 个完整代码示例

#### 验收标准

```
☐ Slave 偏差 <10µs (P99) 
☐ 锁定时间 <30 秒
☐ 无时间倒流现象
☐ 24 小时无错误运行
☐ 在 Ethernet + WiFi 上验证
☐ 所有文档翻译成中英文
```

#### 评价指标

```
同步精度:   ±10 µs (目标达成) ✓
稳定性:     >99.9%
锁定时间:   <30 秒
漂移速率:   <1 µs/分钟
```

---

### M2.3: TSN Alpha 版本 (2026-08-31)

**目标**: TSN 调度框架实现，基本门禁表配置

#### 交付物清单

- [x] **API 和核心库**
  - `libs/tsn_api.h` (C 语言 API)
  - `libs/tsn_stream.c` (流管理)
  - `libs/tsn_gate.c` (门禁调度)
  - `libs/tsn_shaper.c` (流整形)

- [x] **Linux 配置工具**
  - `tools/tsn_setup.sh` (自动配置脚本)
  - `tools/tsn_config.py` (Python 配置工具)
  - `tools/tsn_monitor.py` (实时监控)

- [x] **测试程序**
  - `tests/test_tsn_single_host.sh` (单机测试)
  - `tests/test_tsn_network.sh` (网络拓扑)
  - `tests/test_tsn_latency.py` (延迟测量)

- [x] **文档**
  - `docs/TSN_DESIGN.md` (详细设计)
  - `docs/TSN_CONFIGURATION.md` (配置指南)
  - `libs/tsn/README.md` (API 说明)

#### 验收标准

```
☐ TSN 流创建/删除 API 工作正常
☐ 门禁表配置和激活
☐ tc taprio 配置成功
☐ 基本延迟 <2ms (初步)
☐ 无配置冲突
☐ 文档完整
```

#### 评价指标

```
配置延迟:   <5 秒 (启用 TSN)
流支持:     >50 个流
延迟精度:   <2 ms (初步)
门禁周期:   1 ms (可配置)
```

---

### M2.4: TSN Beta 版本 (2026-09-30)

**目标**: 完整的 TSN 实现，多流隔离验证

#### 交付物清单

- [x] **完整实现**
  - 高级流过滤 (tc flower 规则)
  - 令桶整形算法
  - 优先级队列管理
  - 动态流添加/删除

- [x] **硬件适配**
  - Marvell 交换机测试
  - Realtek 交换机兼容性检查
  - NIC 驱动集成 (Intel i210 可选)

- [x] **性能和可靠性**
  - `tests/test_tsn_stress_1hour.sh` (压力测试)
  - `tests/test_tsn_fault_injection.sh` (故障注入)
  - `tests/test_tsn_multiflow.py` (多流隔离)
  - 性能报告: `docs/TSN_PERFORMANCE_REPORT.md`

- [x] **高级功能**
  - 故障检测和恢复
  - 自动重新配置
  - 实时统计和告警
  - Web 管理界面 (可选)

- [x] **文档**
  - `docs/TSN_TROUBLESHOOTING.md` (故障排查)
  - `docs/TSN_HARDWARE_COMPATIBILITY.md` (硬件支持矩阵)
  - `docs/TSN_DEPLOYMENT_GUIDE.md` (部署指南)
  - 5 个完整应用示例

#### 验收标准

```
☐ 电机控制延迟 <500 µs
☐ 视频传输无丢包 (24 小时)
☐ 多流隔离 >99.9%
☐ 故障自动恢复 <5 秒
☐ Marvell/Realtek 兼容
☐ 所有文档完整
```

#### 评价指标

```
延迟 (P99):         <1 ms ✓
丢包率:             <0.01%
流支持:             >100
配置复杂度:         简单 (自动化工具)
```

---

### M2.5: 硬件参考设计完成 (2026-10-31)

**目标**: PCB 原型完成并验证通过，所有文件开源发布

#### 交付物清单

- [x] **硬件设计文件 (KiCad)**
  - `hardware/openneuro-zone-controller/` (完整项目)
  - 原理图 (.sch)
  - PCB 布局 (.kicad_pcb)
  - 库和符号 (symbols, footprints)

- [x] **生产文件**
  - Gerber 文件 (CAM 数据)
  - BOM.csv (物料清单)
  - Assembly_Guide.pdf (组装指南)
  - DFM_Report.pdf (制造工艺检查)

- [x] **硬件验证数据**
  - `hardware/Testing_Report.md` (测试报告)
  - 原型照片 (×5)
  - 热成像验证
  - 可靠性测试数据

- [x] **文档**
  - `hardware/REFERENCE_DESIGN.md` (本文档)
  - `hardware/Assembly_Instructions.pdf`
  - `hardware/Troubleshooting_Guide.md`
  - Neuro-Link 标准接口规范

- [x] **开源发布**
  - CERN Open Hardware License v2
  - GitHub 项目完整开源
  - 采购指南和成本分析

#### 验收标准

```
☐ 10 块原型 PCB 完成
☐ 所有主要功能验证通过
☐ 电源、网络、I/O 正常
☐ PTP 客户端同步 <10µs
☐ Gigabit 网络吞吐 >900 Mbps
☐ 工作温度 0-70°C 验证
☐ 开源许可证和文档完整
☐ 成本 <$350 (1000 套)
```

#### 规格达成

```
处理能力:   ✓ 32K msg/sec
网络接口:   ✓ 2x Gigabit
延迟:       ✓ <1ms (PTP 同步)
I/O:        ✓ 4x SPI, 8x I2C, 16x GPIO
功耗:       ✓ <5W 运行
尺寸:       ✓ 100x100x25 mm
成本:       ✓ <$350 BOM
```

---

## 📅 详细月度计划

### 第 1 个月: 2026 年 4 月

**周 1-4: PTP 协议栈开发**

```
周 1: 项目启动
├─ [x] 团队分工确认
├─ [x] 开发环境搭建
├─ [x] 代码仓库初始化
└─ [x] 每日站会开始

周 2-3: 协议实现
├─ [x] Master 消息发送 (SYNC/FOLLOW_UP)
├─ [x] Slave 消息接收
├─ [x] 基础时间偏差计算
├─ [x] 状态机实现
└─ [x] 单元测试

周 4: 硬件集成准备
├─ [x] STM32 以太网时间戳驱动框架
├─ [x] 初步集成测试
├─ [x] 性能基准测试
└─ [x] 月度评审
```

**交付**: PTP Alpha 代码框架

---

### 第 2 个月: 2026 年 5 月

**周 5-8: PTP 硬件集成**

```
周 5: STM32H7 集成
├─ [x] 以太网 MAC 时间戳驱动完成
├─ [x] 时间戳精度验证
├─ [x] 集成测试 (Master + STM32 Slave)
└─ [x] Bug 修复和优化

周 6: RK3588 集成
├─ [x] Linux CLOCK_TAI 驱动
├─ [x] 时钟调整实现
├─ [x] 集成测试
└─ [x] 性能评估

周 7: 多节点测试
├─ [x] 3+ 个 Slaves 同步测试
├─ [x] 精度验证 (<100µs)
├─ [x] 稳定性测试 (6 小时)
└─ [x] 性能报告

周 8: M2.1 发布准备
├─ [x] 代码清理和文档
├─ [x] 测试覆盖率检查
├─ [x] 发布候选版本 (RC1)
└─ [x] 月度评审
```

**交付**: M2.1 PTP Alpha 版本

---

### 第 3 个月: 2026 年 6 月

**周 9-12: PTP 优化和验证**

```
周 9-10: 性能优化
├─ [x] PI 伺服参数调优
├─ [x] 频率微调实现和测试
├─ [x] 步进调整机制
├─ [x] CPU 占用优化
└─ [x] 内存优化

周 11: ESP32 集成
├─ [x] FreeRTOS 定时器实现
├─ [x] WiFi 延迟补偿
├─ [x] 集成测试
└─ [x] 稳定性验证

周 12: 验证和发布
├─ [x] 24 小时稳定性测试
├─ [x] 多平台兼容性验证
├─ [x] 故障恢复测试
├─ [x] M2.2 发布
└─ [x] 社区公告
```

**交付**: M2.2 PTP Beta 版本

---

### 第 4-5 个月: 2026 年 7-8 月

**TSN 阶段 1: API 设计和初步实现**

```
周 13-16: TSN API 开发
├─ [x] API 设计审查
├─ [x] C 语言接口实现
├─ [x] Python 绑定 (可选)
├─ [x] 单元测试
└─ [x] 文档编写

周 17-20: Linux 驱动集成
├─ [x] tc taprio 配置工具
├─ [x] tc flower 流分类
├─ [x] 令桶整形器
├─ [x] 监控工具开发
└─ [x] 基础功能测试
```

**交付**: M2.3 TSN Alpha 版本

---

### 第 6-7 个月: 2026 年 9 月

**TSN 阶段 2: 完整实现和验证**

```
周 21-24: 高级功能
├─ [x] 动态流管理
├─ [x] 优先级隔离
├─ [x] 故障检测和恢复
├─ [x] 自动重新配置
└─ [x] Web 界面 (可选)

周 25-28: 硬件测试
├─ [x] Marvell 交换机测试
├─ [x] Realtek 兼容性检查
├─ [x] 多流压力测试
├─ [x] 故障注入测试
└─ [x] M2.4 发布
```

**交付**: M2.4 TSN Beta 版本

---

### 第 8-9 个月: 2026 年 10-11 月

**硬件设计完成**

```
周 29-32: PCB 设计 (4 周)
├─ [x] 原理图完成和审查
├─ [x] PCB 布局和 DFM
├─ [x] 小批量制造 (10 块)
└─ [x] 硬件验证

周 33-36: 集成和优化 (4 周)
├─ [x] 固件开发 (U-Boot, Kernel)
├─ [x] 驱动开发 (Ethernet, I2C, SPI)
├─ [x] Zenoh + ROS 2 集成
├─ [x] 性能测试
└─ [x] M2.5 发布
```

**交付**: M2.5 硬件参考设计

---

## 🎓 技术验收检查清单

### PTP 技术验收

```
核心功能:
  ☐ Master 节点能发送 SYNC 消息
  ☐ Slave 节点能接收和处理
  ☐ 时间偏差计算正确
  ☐ PI 伺服控制正常
  ☐ 频率微调有效
  ☐ 步进调整无时间跳跃
  
性能指标:
  ☐ 同步精度 <10µs (P99)
  ☐ 锁定时间 <30 秒
  ☐ 漂移速率 <1µs/分钟
  ☐ CPU 占用 <1%
  ☐ 内存使用 <20 KB
  
可靠性:
  ☐ 24 小时无中断运行
  ☐ Master 故障自动检测
  ☐ Slave 故障快速恢复
  ☐ 网络延迟抖动恢复 <5 秒
  
平台支持:
  ☐ STM32H7 (Ethernet)
  ☐ RK3588 (Linux)
  ☐ ESP32 (WiFi)
  ☐ Jetson (可选)
```

### TSN 技术验收

```
核心功能:
  ☐ 流创建和销毁
  ☐ 门禁表配置和激活
  ☐ 流分类 (tc flower)
  ☐ 优先级队列调度
  ☐ 流量整形 (令桶)
  ☐ 统计和监控
  
性能指标:
  ☐ 最大延迟 <1ms (P99)
  ☐ 丢包率 <0.01%
  ☐ 支持流数 >100
  ☐ 抖动 <100µs
  
隔离性:
  ☐ 高优先级不被低优先级影响
  ☐ 超额流不会破坏其他流
  ☐ 故障流隔离
  
硬件支持:
  ☐ Marvell 88E6190+ (完整)
  ☐ Realtek RTL8365 (兼容)
  ☐ 标准 Linux tc/taprio
```

### 硬件验收

```
功能验收:
  ☐ 电源启动正常
  ☐ Ethernet 链接 (2x port)
  ☐ GPIO 输入/输出
  ☐ PWM 输出
  ☐ SPI/I2C 通讯
  ☐ UART 串行端口
  ☐ USB (DFU 固件更新)
  
性能验收:
  ☐ Gigabit 网络吞吐 >900 Mbps
  ☐ PTP 同步精度 <10µs
  ☐ CPU 占用 <30% (正常工作)
  ☐ 功耗 <5W 运行
  ☐ 启动时间 <30 秒
  
环境验收:
  ☐ 工作温度 0-70°C
  ☐ 工作湿度 10-90% RH
  ☐ 静电防护 IEC 61000-4-2
  ☐ 振动耐受 MIL-STD-810H
  
设计验收:
  ☐ 所有原理图评审通过
  ☐ PCB DFM 检查通过
  ☐ BOM 成本 <$350
  ☐ 开源许可证完整
```

---

## 📊 质量指标

### 代码质量

```
指标              目标        当前    验收
─────────────────────────────────────
测试覆盖率        >90%        TBD     ☐
代码审查率        100%        TBD     ☐
编译警告          0           TBD     ☐
静态分析级别      A           TBD     ☐
```

### 文档完整性

```
文档类型          完成度      状态
─────────────────────────────────────
API 参考          100%        ☐
集成指南          100%        ☐
故障排查          100%        ☐
硬件说明          100%        ☐
示例代码          5 个        ☐
```

### 社区参与

```
指标              目标    当前
─────────────────────────────
GitHub Star       100+    TBD
Fork              20+     TBD
Issue (resolved)  50+     TBD
贡献者            5+      TBD
```

---

## 🚀 发布流程

### 版本号规划

```
v0.2.0 (2026-12-15) - 阶段二完整发布
├─ v0.2.0-alpha.1 (2026-05-31) = M2.1
├─ v0.2.0-beta.1 (2026-06-30) = M2.2
├─ v0.2.0-beta.2 (2026-08-31) = M2.3
├─ v0.2.0-rc.1 (2026-09-30) = M2.4
└─ v0.2.0 (2026-12-15) = 最终版本

后续规划:
├─ v0.2.1 (2027-01-15) - Bug 修复
├─ v0.3.0 (2027-03-15) - 冗余和故障转移
└─ v1.0.0 (2027-06-15) - 生产就绪
```

### 发布检查清单

```
代码:
  ☐ 所有测试通过
  ☐ 代码审查完成
  ☐ 发布分支创建
  ☐ 版本标签设置

文档:
  ☐ API 文档完整
  ☐ CHANGELOG.md 更新
  ☐ README.md 更新
  ☐ 发布说明编写

构件:
  ☐ Docker 镜像构建
  ☐ 源码压缩包生成
  ☐ 签名和校验和生成

宣传:
  ☐ GitHub Release 发布
  ☐ 社区公告
  ☐ 技术博客发表
  ☐ 演讲视频录制
```

---

## 📈 成功指标总结

### 技术成功

| 指标 | 目标 | M2.2 | M2.4 | M2.5 |
|------|------|------|------|------|
| **PTP 精度** | <10µs | ✓ | ✓ | ✓ |
| **TSN 延迟** | <1ms | - | ✓ | ✓ |
| **代码行数** | >5000 | 2500 | 4500 | 5500 |
| **文档页数** | >50 | 20 | 40 | 60 |
| **支持硬件** | 4+ | 3 | 4 | 5+ |

### 业务成功

```
社区贡献者:     5+ 人
GitHub Star:    100+ 个
企业采用:       3+ 家
学术论文:       2+ 篇
演讲次数:       5+ 次
```

---

## 📞 联系和支持

**项目主页**: https://github.com/OpenRobotics/OpenNeuro  
**讨论社区**: https://github.com/OpenRobotics/OpenNeuro/discussions  
**问题追踪**: https://github.com/OpenRobotics/OpenNeuro/issues  
**月度更新**: https://blog.openneuro.io/

---

**里程碑与交付清单完成！** ✅

此文档定义了 Stage 2 的完整验收标准和发布计划。

下一步: 启动社区推广和募资计划 (M3)

