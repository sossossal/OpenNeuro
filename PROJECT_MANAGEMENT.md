# 项目管理和跟踪指南

## GitHub Projects 看板配置

### W2 编码冲刺看板

**列表配置**:

#### 📋 Backlog (待处理)
- 未分配给当前冲刺的任务
- 待优先级排序
- 等待需求澄清

#### 🔄 In Progress (进行中)
- 开发人员正在处理的任务
- 已开始编码
- 正在代码审查中

#### 👀 In Review (审查中)
- 已提交 PR，等待审查
- 2/3 审查人员已签署
- 等待 CI/CD 通过

#### ✅ Done (完成)
- 已合并到 develop
- 测试通过
- 文档更新完毕
- CI/CD 绿灯

### 任务分配

**PTP Team**:
- [ ] ANNOUNCE 消息实现 (50 LOC) - Lead-PTP
- [ ] SYNC 消息实现 (40 LOC) - Developer-PTP-1
- [ ] 消息序列化 (30 LOC) - Developer-PTP-2
- [ ] 单元测试 (60 LOC) - Developer-PTP-1,2

**TSN Team**:
- [ ] 优先级队列实现 (70 LOC) - Lead-TSN
- [ ] 调度器框架 (60 LOC) - Developer-TSN-1
- [ ] Linux 集成 (40 LOC) - Developer-TSN-2
- [ ] 性能基准测试 (50 LOC) - Developer-TSN-1,2

**Hardware Team**:
- [ ] 电源系统原理图 (50% 完成) - Lead-HW
- [ ] 网络接口设计 (开始) - Developer-HW-1
- [ ] 库管理和验证 - Developer-HW-2

**QA Team**:
- [ ] CI/CD 流水线验证 - Lead-QA
- [ ] 覆盖率监控部署 - Developer-QA-1
- [ ] 性能基准工具 - Developer-QA-2

## 里程碑管理

### M2.1 (2026-02-16) - Alpha 发布

**目标**:
- PTP 基本消息处理
- TSN 优先级调度
- 硬件参考设计
- 完整测试覆盖

**交付物**:
- ✓ 代码: 500+ LOC (PTP) + 400+ LOC (TSN)
- ✓ 测试: 覆盖率 ≥ 80%
- ✓ 文档: API 文档 100%
- ✓ 社区: GitHub Star ≥ 50

**验收标准**:
- [ ] 所有功能测试通过
- [ ] 代码审查批准
- [ ] 性能基准达成
- [ ] 文档完整

### M2.2 (2026-03-16) - 性能优化

**目标**:
- PTP 延迟 <5µs
- TSN 多网络支持
- 硬件扩展卡

### M2.3 (2026-04-27) - 企业特性

**目标**:
- Web UI 监控
- OpenMetrics 接口
- RAFT 集群

### M2.4 (2026-05-25) - 高可用

**目标**:
- 99.99% SLA
- 灾难恢复
- 企业集成

### M2.5 (2026-06-30) - AI 驱动

**目标**:
- AI 优化
- 异常检测
- 社区发布 (v1.0)

## Issue 管理

### 优先级标签

- **P0 (Critical)**: 阻塞发布，立即修复
- **P1 (High)**: 影响功能，本周修复
- **P2 (Medium)**: 改进功能，本月修复
- **P3 (Low)**: 优化建议，无期限

### 状态标签

- **status/blocked**: 被其他任务阻塞
- **status/review**: 等待代码审查
- **status/testing**: 等待测试
- **status/docs**: 需要文档更新

### 技术标签

- **area/ptp**: PTP 堆栈
- **area/tsn**: TSN 调度器
- **area/hw**: 硬件设计
- **area/ci-cd**: CI/CD 流水线
- **area/docs**: 文档

## 周报告流程

### 每周五 14:00-16:00 技术评审

**议程**:

1. **统计数据** (5 min)
   - 完成的 Issue: N
   - 关闭的 PR: N
   - 新增代码行: N
   - 测试覆盖率: N%

2. **功能演示** (20 min)
   - PTP: 实现了什么
   - TSN: 实现了什么
   - HW: 设计进展
   - QA: 工具链更新

3. **问题讨论** (20 min)
   - 技术难点
   - 风险评估
   - 资源需求
   - 时间表调整

4. **下周计划** (10 min)
   - 各团队目标
   - 里程碑进度
   - 关键路径

5. **其他事项** (5 min)

### 周报告模板

```markdown
# W2 技术评审报告 - 2026-02-08

## 统计数据
- 完成 Issue: 12
- 关闭 PR: 8
- 新增代码: 450 LOC
- 覆盖率: 83%

## PTP Team 进展
- ✓ ANNOUNCE 消息 100% 完成
- ✓ 单元测试覆盖 85%
- → SYNC 消息开发中 (60% 完成)

## TSN Team 进展
- ✓ 优先级队列 100% 完成
- ✓ 基准测试通过 (<1µs)
- → 调度器集成中 (40% 完成)

## Hardware Team 进展
- → 电源系统 50% 完成
- 下周: 网络接口开始

## QA Team 进展
- ✓ CI/CD 流水线运行正常
- ✓ 覆盖率自动报告配置完成
- → 性能基准工具集成中

## 风险评估
- LOW: 所有关键路径按计划进行

## 下周目标
- PTP: SYNC 消息完成
- TSN: 调度器基本完成
- HW: 网络接口原理图
- QA: 性能基准工具完成
```

## 沟通工具

### Slack 频道结构

```
#announcements           公告
├─ #dev-status          日常进展报告
├─ #dev-ptp             PTP 技术讨论
├─ #dev-tsn             TSN 技术讨论
├─ #dev-hardware        硬件设计讨论
├─ #dev-qa              测试 & CI/CD
└─ #dev-random          随意讨论
```

### 同步会议

| 会议 | 时间 | 参与者 | 时长 |
|------|------|--------|------|
| Daily Standup | 10:00 | 全体 | 15 min |
| Technical Review | Fri 14:00 | 全体 | 120 min |
| 1-on-1 (可选) | 灵活 | Lead + Dev | 30 min |

## 指标跟踪

### 每周指标

- **代码提交数**: 目标 ≥ 50
- **PR 数量**: 目标 ≥ 15
- **代码审查**: 目标 ≥ 30
- **CI/CD 通过率**: 目标 ≥ 95%
- **测试覆盖率**: 目标 ≥ 80%
- **缺陷密度**: 目标 < 2/KLOC

### 月度指标

- **功能完成度**: M2.1 目标 100%
- **社区关注**: GitHub Star 增长
- **文档完整度**: API 文档 100%
- **代码质量**: 代码审查通过率 100%

## 学习资源

- [GitHub 最佳实践](https://github.blog/category/engineering/)
- [Semantic Versioning](https://semver.org/)
- [Keep a Changelog](https://keepachangelog.com/)
- [Conventional Commits](https://www.conventionalcommits.org/)
