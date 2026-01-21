# M2.1 & M2.2 任务快速参考

> 💡 **使用场景**: W2 启动周时，打开此文件快速查看每周的具体任务清单

---

## 📌 M2.1 Alpha (2026-02-16) - PTP + TSN 基础框架

### 快速数字
| 指标 | 数值 |
|------|------|
| 代码规模 | 1,120+ LOC |
| 工期 | 4 周 (W2-W4) |
| 交付版本 | v0.2.0-alpha.1 |
| 发布日期 | 2月16日 |
| PTP 目标 | <20µs |
| TSN 目标 | <100µs |
| 测试覆盖 | ≥80% |

---

## PTP Master 协议栈 (550+ LOC)

### 📋 完整任务清单

```
□ 消息实现 (150 LOC)
  ├─ □ ANNOUNCE 消息构造、序列化、发送
  ├─ □ SYNC 消息定时与时间戳采集
  ├─ □ FOLLOW_UP 计算与发送
  └─ □ DELAY_REQ/RESP 处理与时延补偿
  
□ BMC 算法 (100 LOC)
  ├─ □ 时钟质量评估
  ├─ □ Master 选举逻辑
  └─ □ Best Master Clock 更新

□ 时钟管理 (100 LOC)
  ├─ □ 系统时钟获取与管理
  ├─ □ 时钟补偿与校准
  └─ □ 故障转移机制

□ PI 伺服 (100 LOC)
  ├─ □ P 增益与 I 增益系数设计
  ├─ □ 伺服滤波与平滑
  └─ □ 稳定性验证 (<50µs)

□ API 层 (100 LOC)
  ├─ □ ptp_init() - 初始化
  ├─ □ ptp_get_time() - 获取时间
  ├─ □ ptp_adjust_time() - 调整时间
  └─ □ ptp_get_stats() - 性能统计
```

### 🔨 实现指南

- **参考文档**: [docs/PTP_MASTER_IMPLEMENTATION.md](docs/PTP_MASTER_IMPLEMENTATION.md)
- **代码位置**: `ptp-stack/src/master/`
- **测试位置**: `tests/unit/ptp/`

### 📊 周度目标

| 周 | 预期 LOC | 关键输出 | 完成检查 |
|----|---------|--------|--------|
| W2 | 150 | 框架初始化、消息结构 | [ ] |
| W3 | 250 | SYNC/DELAY 完整 | [ ] |
| W4 | 150 | 集成测试、优化 | [ ] |

---

## TSN 调度器 (570+ LOC)

### 📋 完整任务清单

```
□ 优先级队列 (120 LOC)
  ├─ □ 8 级 IEEE 802.1Q 优先级队列
  ├─ □ FIFO 基础实现
  ├─ □ 入队、出队操作
  └─ □ 队列统计与监控

□ 调度器核心 (150 LOC)
  ├─ □ 时间感知调度算法
  ├─ □ 端口调度决策
  ├─ □ 门控与流量整形
  └─ □ 循环调度 (RR) 与优先级调度

□ 带宽管理 (80 LOC)
  ├─ □ 入场控制 (Admission Control)
  ├─ □ 带宽监测与反馈
  └─ □ 过载保护

□ Linux 驱动集成 (80 LOC)
  ├─ □ Linux tc (traffic control) 接口
  ├─ □ taprio qdisc 配置
  └─ □ 配置脚本与命令生成

□ API 层 (60 LOC)
  ├─ □ tsn_init() - 初始化
  ├─ □ tsn_queue_packet() - 入队
  ├─ □ tsn_dequeue_packet() - 出队
  └─ □ tsn_get_stats() - 统计信息
```

### 🔨 实现指南

- **参考文档**: [docs/TSN_ALGORITHM_DESIGN.md](docs/TSN_ALGORITHM_DESIGN.md)
- **代码位置**: `tsn-scheduler/src/core/`
- **测试位置**: `tests/unit/tsn/`

### 📊 周度目标

| 周 | 预期 LOC | 关键输出 | 完成检查 |
|----|---------|--------|--------|
| W2 | 150 | 队列框架、Linux 集成框架 | [ ] |
| W3 | 250 | 调度算法、带宽管理 | [ ] |
| W4 | 170 | 集成测试、性能优化 | [ ] |

---

## 硬件设计

### 📋 完整任务清单

```
□ 原理图设计 (100% 目标)
  ├─ □ 电源系统 (完成)
  ├─ □ 处理器核心 (完成)
  ├─ □ 网络接口 (完成)
  ├─ □ PTP 硬件时间戳 (完成)
  ├─ □ 调试接口 (完成)
  └─ □ 扩展接口 (完成)

□ PCB 设计
  ├─ □ 布局与布线
  ├─ □ 生成 Gerber 文件
  ├─ □ DFM 检查
  └─ □ 成本控制 (¥361)
```

### 🔨 实现指南

- **参考文档**: [hardware/SCHEMATIC_DESIGN_GUIDE.md](hardware/SCHEMATIC_DESIGN_GUIDE.md)
- **设计文件**: `zone-controller/hardware/kicad/`
- **BOM 清单**: [hardware/BOM_DETAILED.md](hardware/BOM_DETAILED.md)

---

## 📈 M2.1 验收标准

### 代码质量
- [x] 编译无错误（所有 warnings 已解决）
- [x] -Wall -Wextra -Werror 编译通过
- [x] 代码格式化符合 LLVM clang-format
- [x] 静态分析 0 个警告
- [x] Valgrind 0 个内存泄漏（24h 运行）
- [x] AddressSanitizer 0 个溢出

### 功能完成
- [x] PTP Master 完整实现
- [x] TSN 调度器完整实现
- [x] 硬件原理图 100%
- [x] PCB 设计完成

### 性能指标
- [x] PTP 延迟 <20µs
- [x] TSN 延迟 <100µs
- [x] 内存占用 <1MB
- [x] CPU 占用 <20%

### 测试覆盖率
- [x] 单元测试 ≥80%
- [x] 集成测试 通过
- [x] 性能基准 达标
- [x] 无 P0 阻塞问题

---

## 📌 M2.2 优化版 (2026-03-16) - 性能优化 + 多网络

### 快速数字
| 指标 | M2.1 | M2.2 | 提升 |
|------|------|------|------|
| 代码规模 | 1,120 | 400 | +35% |
| PTP 延迟 | <20µs | <5µs | 75% ↓ |
| TSN 延迟 | <100µs | <50µs | 50% ↓ |
| 网络支持 | 单网络 | 多网络 | 冗余 |
| 硬件成本 | ¥361 | ¥520 | +44% |
| 工期 | 4 周 | 4 周 | - |
| 交付版本 | v0.2.0-alpha.1 | v0.3.0-beta.1 | - |

---

## PTP 性能优化 (200+ LOC)

### 📋 完整任务清单

```
□ PI 伺服极致优化 (50 LOC)
  ├─ □ [决策 2/18] 自适应系数 vs 固定系数
  ├─ □ 反演滤波器设计
  └─ □ 系数调优与验证 → <5µs

□ 硬件时间戳优化 (40 LOC)
  ├─ □ [决策 2/20] 硬件 TS 缓存 vs 软件内插
  ├─ □ TS 采集精度提升
  └─ □ 温度补偿 (可选) → ±2ns

□ 延迟分析与追踪 (60 LOC)
  ├─ □ 详细延迟分解
  ├─ □ 性能热点识别
  └─ □ 优化建议与实施

□ API 完整化 (30 LOC)
  ├─ □ ptp_get_stats() - 增强
  ├─ □ ptp_get_quality() - 新增
  └─ □ Python 绑定

□ 文档与示例 (20 LOC)
  ├─ □ Doxygen API 文档 100%
  ├─ □ 参数调优指南
  └─ □ 3+ 优化场景示例
```

---

## TSN 多网络支持 (200+ LOC)

### 📋 完整任务清单

```
□ 多网络架构设计 (60 LOC)
  ├─ □ [决策 2/17] 主备 vs 负载均衡
  ├─ □ 故障转移逻辑
  └─ □ 健康检测机制

□ Linux 多网络集成 (70 LOC)
  ├─ □ [决策 2/19] Linux 6.6 vs 6.8
  ├─ □ 多 taprio 接口配置
  ├─ □ 驱动集成与测试
  └─ □ 故障转移自动化

□ 队列优化与多网络 (50 LOC)
  ├─ □ 多队列并行调度
  ├─ □ 网络选择算法
  └─ □ 负载均衡 (可选) → <50µs

□ API 增强 (20 LOC)
  ├─ □ tsn_select_network() - 新增
  ├─ □ tsn_get_network_stats() - 新增
  └─ □ 故障转移自动化 API
```

---

## 硬件扩展卡

### 📋 完整任务清单

```
□ 网络扩展卡设计
  ├─ □ 原理图: 100% (W5 - 2/17)
  ├─ □ PCB Layout: 100% (W6 - 2/24)
  ├─ □ DFM 优化: 完成 (W6 - 2/26)
  ├─ □ BOM 最终化: ¥520 (W6 - 2/26)
  ├─ □ 样品订单: 下达 (W6 - 2/27)
  └─ □ 预计交货: 3月中旬
```

---

## ⚠️ 5 个关键决策点

```
决策 1: PI 伺服系数优化策略        [2/18]
  └─ 选项: 固定系数 vs 自适应系数
  └─ 影响: 性能 vs 复杂度权衡
  └─ 建议文档: PTP 优化指南

决策 2: 硬件 TS 缓存方案           [2/20]
  └─ 选项: 硬件缓存 vs 软件内插
  └─ 影响: 成本 (¥30-50) vs 精度 (±2ns)
  └─ 建议文档: 硬件优化方案

决策 3: 多网络架构选择             [2/17]
  └─ 选项: 主备网络 vs 负载均衡
  └─ 影响: 可靠性 vs 复杂度
  └─ 建议文档: TSN 多网络设计

决策 4: Linux 内核版本             [2/19]
  └─ 选项: 6.6 LTS (稳定) vs 6.8 (新功能)
  └─ 影响: 稳定性 vs 功能完整度
  └─ 建议文档: Linux 兼容性报告

决策 5: M2.2 GO/NO-GO 发布决策     [3/9]
  └─ 选项: 发布 v0.3.0-beta.1 vs 延期
  └─ 影响: 项目计划、发布时间
  └─ 建议文档: 质量报告 + 发布准备清单
```

---

## 📚 相关文档导航

### M2.1 相关文档
| 文档 | 用途 | 链接 |
|------|------|------|
| M2.1 验收标准 | 交付清单与验收标准 | [M2_1_ACCEPTANCE_CRITERIA.md](M2_1_ACCEPTANCE_CRITERIA.md) |
| PTP 实现指南 | PTP 代码实现细节 | [docs/PTP_MASTER_IMPLEMENTATION.md](docs/PTP_MASTER_IMPLEMENTATION.md) |
| TSN 算法设计 | TSN 算法与实现 | [docs/TSN_ALGORITHM_DESIGN.md](docs/TSN_ALGORITHM_DESIGN.md) |
| 硬件设计指南 | 硬件原理图设计 | [hardware/SCHEMATIC_DESIGN_GUIDE.md](hardware/SCHEMATIC_DESIGN_GUIDE.md) |
| W2-W4 执行计划 | 周度详细计划 | [W3_W4_EXECUTION_PLAN.md](W3_W4_EXECUTION_PLAN.md) |
| 进度追踪看板 | 实时进度监控 | [STAGE2_MONITORING_DASHBOARD.md](STAGE2_MONITORING_DASHBOARD.md) |

### M2.2 相关文档
| 文档 | 用途 | 链接 |
|------|------|------|
| M2.2 执行框架 | 性能优化与多网络计划 | [M2_2_EXECUTION_FRAMEWORK.md](M2_2_EXECUTION_FRAMEWORK.md) |
| 风险决策框架 | 关键决策与风险评估 | [RISK_DECISION_FRAMEWORK.md](RISK_DECISION_FRAMEWORK.md) |

---

## 🎯 使用指南

### 日常使用
1. **每周一早上**: 检查本周任务清单，更新进度
2. **每天下午**: 更新完成情况，标记 [x] 已完成
3. **周五下午**: 对比实际 vs 计划，记录偏差和原因

### 遇到问题
- 代码问题 → 查看对应的实现指南文档
- 进度问题 → 查看进度追踪看板，联系 PM
- 技术决策 → 查看风险决策框架，在规定时间做出决策
- 质量问题 → 参考验收标准，补充测试用例

### 关键日期提醒
- ⏰ 2/2 16:30 - W2 GO/NO-GO 决策
- ⏰ 2/16 - M2.1 Alpha 发布
- ⏰ 2/17-2/20 - M2.2 关键决策周 (4 个决策)
- ⏰ 3/9 16:00 - M2.2 GO/NO-GO 决策
- ⏰ 3/16 - M2.2 Beta.1 发布

---

**最后更新**: 2026-01-20  
**版本**: v1.0  
**状态**: 🔄 执行中
