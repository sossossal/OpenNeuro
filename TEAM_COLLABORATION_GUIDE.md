# Stage 2 - 团队协作与通信指南

**版本**: v1.0  
**日期**: 2026-01-20  
**用途**: W2-W4 (4周) 团队协调、沟通流程、知识共享  
**适用**: 全员 (PTP Team, TSN Team, HW Team, PM)

---

## 📋 目录

1. [团队结构与职责](#团队结构与职责)
2. [日常沟通流程](#日常沟通流程)
3. [会议流程与模板](#会议流程与模板)
4. [知识共享与文档](#知识共享与文档)
5. [问题报告与反馈](#问题报告与反馈)

---

## 团队结构与职责

### 核心团队配置

```
Project Manager
    ├─ PTP Team Lead (Alice)
    │   ├─ PTP Developer 1 (消息处理)
    │   ├─ PTP Developer 2 (BMC 算法 + 时钟)
    │   └─ Test Engineer (单元测试)
    │
    ├─ TSN Team Lead (Bob)
    │   ├─ TSN Developer 1 (队列 + 调度)
    │   ├─ TSN Developer 2 (Linux 集成)
    │   └─ Test Engineer (集成测试)
    │
    └─ Hardware Team Lead (Charlie)
        ├─ Schematic Designer
        ├─ PCB Designer
        └─ Hardware Test Engineer
```

### 角色与职责定义

```
【Project Manager - PM】
职责:
  ├─ W2-W4 整体进度管理
  ├─ 周度进度审查与汇报
  ├─ 风险识别与缓解决策
  ├─ 跨团队协调与问题升级
  ├─ M2.1 发布计划与推进
  └─ 团队资源分配与优化

汇报链条: 直接向项目赞助商汇报
参加会议:
  ├─ 每日 10:00 站会 (15 min) - 参加
  ├─ 每周 14:00 技术评审 (120 min) - 主持
  └─ 临时风险评审会 - 主持

【PTP Team Lead - Alice】
职责:
  ├─ PTP Master 完整实现 (550 LOC)
  ├─ SYNC/DELAY/ANNOUNCE 消息流程设计与实现
  ├─ BMC 算法实现与测试
  ├─ 时钟同步与精度控制
  ├─ PTP 性能基准测试
  ├─ 团队成员工作分配与指导
  └─ 代码审查与质量把控

汇报链条: 向 PM 汇报
参加会议:
  ├─ 每日 10:00 站会 (15 min) - 参加
  ├─ 每周 14:00 技术评审 (30 min) - 进度报告
  └─ 每周五 15:00 PTP 设计会 (30 min) - 讨论设计问题

可以做决策:
  ├─ 技术设计细节 (API 改动、消息格式)
  ├─ 测试策略 (覆盖率目标、测试场景)
  └─ 代码审查通过/拒绝

【TSN Team Lead - Bob】
职责:
  ├─ TSN Scheduler 完整实现 (570 LOC)
  ├─ 流量分类与优先级映射
  ├─ 带宽预留与控制
  ├─ Linux tc/taprio 集成
  ├─ TSN 性能基准测试
  ├─ 团队成员工作分配与指导
  └─ 代码审查与质量把控

汇报链条: 向 PM 汇报
参加会议:
  ├─ 每日 10:00 站会 (15 min) - 参加
  ├─ 每周 14:00 技术评审 (30 min) - 进度报告
  └─ 每周五 15:30 TSN 设计会 (30 min) - 讨论设计问题

可以做决策:
  ├─ 技术设计细节 (队列结构、调度算法)
  ├─ 测试策略 (覆盖率目标、测试场景)
  └─ 代码审查通过/拒绝

【Hardware Team Lead - Charlie】
职责:
  ├─ Zone Controller 硬件设计 (原理图 + PCB)
  ├─ RK3588 处理器模块选型与集成
  ├─ 网络接口设计与 PTP 硬件时间戳
  ├─ PCB 设计与制造准备
  ├─ 硬件集成与测试
  ├─ 成本控制与 BOM 管理
  └─ 团队成员指导

汇报链条: 向 PM 汇报
参加会议:
  ├─ 每日 10:00 站会 (15 min) - 参加
  ├─ 每周 14:00 技术评审 (30 min) - 进度报告
  └─ 每周二/四 09:00 硬件设计会 - 讨论原理图与 PCB

可以做决策:
  ├─ 器件选型与替代
  ├─ PCB 布局策略
  └─ 硬件测试计划

【Test Engineer / Developer】
职责:
  ├─ 编写单元测试 (target: 80%+ 覆盖率)
  ├─ 集成测试设计与执行
  ├─ 性能基准测试
  ├─ Bug 报告与追踪
  └─ 测试工具与框架维护

汇报链条: 向团队 Lead 汇报
参加会议:
  ├─ 每日 10:00 站会 - 参加
  ├─ 每周 14:00 技术评审 - 测试进度报告
  └─ 测试规划会议 (按需)

可以做决策:
  ├─ 测试用例设计
  ├─ Bug 优先级评估
  └─ 测试工具选型
```

---

## 日常沟通流程

### 沟通工具与用途

```
工具             用途                优先级    响应时间
─────────────────────────────────────────────────────
Slack            日常沟通/快速反馈   高        实时-15min
GitHub Issues    Bug 追踪/任务管理   中        24 小时
GitHub PR        代码审查/讨论       中        24 小时
Email            正式通知/归档记录   低        2 小时
微信             紧急事项/决策       高        实时-5min
```

### Slack 频道使用规范

```
【核心频道】

#general
├─ 用途: 全员公告、重要新闻、会议提醒
├─ 发布: PM, Team Leads
├─ 频率: 每周 2-3 次

#daily-standup
├─ 用途: 每日站会摘要 + 进度更新
├─ 格式: [Team] Day Summary: completed X, in-progress Y, blockers Z
├─ 发布: 10:30 (每天)
├─ 例:
│  [PTP] Day Summary (1/28):
│  ✅ Completed: SYNC message structure (150 LOC)
│  🔄 In-progress: BMC algorithm implementation
│  ⚠️ Blocker: None
│  📊 Daily LOC: +150 (Total: 150/200)

#ptp-team
├─ 用途: PTP 团队内部讨论
├─ 成员: Alice + 2 developers + test engineer
├─ 频率: 按需讨论

#tsn-team
├─ 用途: TSN 团队内部讨论
├─ 成员: Bob + 2 developers + test engineer
├─ 频率: 按需讨论

#hardware-team
├─ 用途: 硬件团队内部讨论
├─ 成员: Charlie + PCB Designer + 硬件工程师
├─ 频率: 按需讨论

#integration
├─ 用途: 跨团队集成问题讨论
├─ 成员: 所有 Team Leads + 技术代表
├─ 频率: W3-W4 每周 2-3 次

#urgent
├─ 用途: 紧急问题、阻塞事项
├─ 规则:
│  └─ 仅用于 P0 问题 (编译崩溃、集成失败等)
│  └─ 发布后 15 分钟内必须有团队响应
│  └─ 解决后 30 分钟内关闭话题
├─ 频率: 应该 <1 次/周
```

### 发布重要信息的流程

```
步骤 1: 准备消息 (5 min)
  ├─ 简洁明了 (2-3 句)
  ├─ 包含关键信息
  ├─ 标记优先级
  └─ 提供行动项 (如果有)

步骤 2: 选择频道 (1 min)
  ├─ 紧急阻塞? → #urgent
  ├─ 全员公告? → #general
  ├─ 团队内部? → #team-channel
  └─ 集成问题? → #integration

步骤 3: 发布 (2 min)
  ├─ 【高优先】格式: [URGENT] 标题 + 原因 + 行动
  ├─ 【中优先】格式: [INFO] 标题 + 细节
  ├─ 【低优先】格式: 标题 + 链接 (详见 GitHub)
  └─ 提及相关人员 @person

步骤 4: 跟踪 (按需)
  ├─ 紧急问题: 15 分钟内确认
  ├─ 重要信息: 1 小时内确认理解
  └─ 一般信息: 无需跟踪

【示例】

紧急问题:
[URGENT] PTP 代码编译崩溃
原因: 头文件循环引用
影响: PTP 开发完全阻塞
行动: @Alice 请评估, @Bob 是否 TSN 受影响?
预计解决: 30 min

重要信息:
[INFO] W3 启动会议推迟至明天 10:00
原因: Charlie 参加硬件供应商会议
新时间: 明天 10:00-12:00 (same Zoom link)
确认: 请在下面回复 ✅
```

---

## 会议流程与模板

### 每日站会 (Daily Standup)

```
时间: 每天 10:00 - 10:15 (15 分钟)
参加: PM + 所有 Team Leads (必须)
      各团队 1-2 代表 (建议)
位置: Zoom 线上或会议室
主持: PM

议程:

【准备】(10:00-10:05)
  ├─ 参加者上线、检查音频
  └─ PM 打开会议记录

【进度更新】(10:05-10:12)
  ├─ PTP Lead (2 min)
  │  ├─ 昨天完成了什么 (具体模块)
  │  ├─ 今天计划什么
  │  └─ 有没有阻塞
  │
  ├─ TSN Lead (2 min) [同上]
  │
  └─ HW Lead (2 min) [同上]

【问题识别】(10:12-10:15)
  ├─ PM 汇总任何需要特别讨论的问题
  ├─ 如有阻塞: 立即安排单独讨论 (15:00)
  └─ 其他问题: 列入周度评审

【会议记录模板】

日期: YYYY-MM-DD
参加: PM, Alice (PTP), Bob (TSN), Charlie (HW)

PTP Team:
  昨日完成: [列出具体模块+LOC]
  今日计划: [列出今日目标]
  阻塞情况: [如无则写"无"]
  代码进度: [已完成 X/200 LOC (W2 目标)]
  覆盖率: [最新数据, e.g., 42%]

TSN Team:
  昨日完成: [...]
  今日计划: [...]
  阻塞情况: [...]
  代码进度: [已完成 X/220 LOC (W2 目标)]
  覆盖率: [最新数据]

Hardware Team:
  昨日完成: [...]
  今日计划: [...]
  阻塞情况: [...]
  原理图进度: [XX%]
  PCB 进度: [XX%]

PM 备注:
  ├─ 发现的风险: [如有]
  ├─ 需要升级讨论: [如有]
  └─ 下一步行动: [列举]
```

### 周度技术评审会 (Weekly Technical Review)

```
时间: 每周五 14:00 - 16:00 (120 分钟)
参加: PM, 所有 Team Leads, 技术代表 (核心贡献者)
位置: Zoom 线上 + 会议室
主持: PM
记录: PM 辅助或委派

议程:

【开场】(14:00-14:05, 5 min)
  ├─ PM 介绍本周主题 (1 个重点 + 2 个风险)
  ├─ 确认所有人参加
  └─ 设定时间预期

【PTP 进度报告】(14:05-14:25, 20 min)
  ├─ PTP Lead 发言 (10 min)
  │  ├─ 本周完成的功能 (模块 + LOC + 代码审查状态)
  │  ├─ 性能基准 (如有测试)
  │  ├─ 单元测试覆盖率 (趋势)
  │  ├─ 发现的问题与解决方案
  │  └─ 下周计划 (高层概览)
  │
  ├─ 技术讨论 (5 min)
  │  ├─ Q&A (任何人可提问)
  │  └─ 设计决策讨论 (如需要)
  │
  └─ PM 评估进度 (5 min)
     ├─ 对标 W2-W4 计划
     └─ 提出任何风险或调整建议

【TSN 进度报告】(14:25-14:45, 20 min)
  └─ [同 PTP 格式]

【硬件设计报告】(14:45-15:05, 20 min)
  ├─ HW Lead 发言 (10 min)
  │  ├─ 本周完成的原理图部分 (页数 + 内容)
  │  ├─ PCB 初步规划 (如开始)
  │  ├─ 发现的电气问题与解决方案
  │  ├─ 成本验证 (BOM 更新)
  │  └─ 下周计划
  │
  └─ [Q&A + 评估同上]

【跨系统集成讨论】(15:05-15:35, 30 min)
  ├─ 交接点与接口确认 (PTP ↔ TSN ↔ Hardware)
  ├─ 集成测试计划 (W4 启动)
  ├─ 已发现的接口问题
  └─ 下周优先级确认

【风险与问题评审】(15:35-15:50, 15 min)
  ├─ PM 汇报当前风险寄存器
  │  ├─ 新增风险
  │  ├─ 状态变化 (低→中, 中→高)
  │  └─ 缓解措施进展
  │
  ├─ 决策点检查 (是否需要立即决策)
  │
  └─ 问题优先级确认 (P0-P3)

【M2.1 进度与交付准备】(15:50-15:55, 5 min)
  ├─ 距离目标还有多远
  ├─ 交付物清单检查
  └─ 发布阻塞 (如有)

【行动项与总结】(15:55-16:00, 5 min)
  ├─ PM 总结本周亮点与挑战
  ├─ 确认下周重点
  ├─ 行动项分配与截止日期
  └─ 下周同时间确认

【会议记录模板】

日期: YYYY-MM-DD
参加: [列举]

PTP 进度:
  ├─ 本周完成: [模块列表 + LOC]
  ├─ 代码审查: [已合并 X PR, 待审 Y PR]
  ├─ 覆盖率: [从 A% → B%]
  ├─ 性能: [PTP 延迟实测 Zµs]
  ├─ 问题: [已解决 X 个, 新增 Y 个]
  ├─ 下周目标: [详细目标]
  └─ 技术债: [累积的 TODO]

TSN 进度: [同上]

硬件进度:
  ├─ 原理图完成度: [XX%]
  ├─ 完成的部分: [Sheet 1, 2, ...]
  ├─ 问题: [电源问题, ...]
  ├─ 下周目标: [XX%]
  └─ PCB 准备: [预计启动时间]

跨系统:
  ├─ 接口问题: [列举]
  ├─ 集成计划: [W3 或 W4 启动]
  └─ 依赖关系: [哪些模块依赖哪些模块完成]

风险评估:
  ├─ 当前总体状态: [🟢 绿/🟡 黄/🔴 红]
  ├─ 高优先级风险: [RISK-001: 描述 + 状态]
  ├─ 新发现的风险: [如有]
  └─ 下周关键决策点: [日期 + 内容]

M2.1 准备:
  ├─ 交付物完成度: [XX%]
  ├─ 关键路径: [最后的障碍]
  └─ 发布日期确认: [2月16日?]

行动项:
  ├─ 【行动】谁 → 什么 → 截止日期
  ├─ 【行动】...
  └─ 【行动】...

下周重点:
  ├─ PTP: [主要目标]
  ├─ TSN: [主要目标]
  ├─ HW: [主要目标]
  └─ 整体: [关键里程碑]
```

---

## 知识共享与文档

### 文档维护责任

```
文档类型               所有者        更新频率      位置
───────────────────────────────────────────────────────
Architecture Docs      各 Team Lead   W1 末完成    docs/
Implementation Docs    各 Team Lead   周度更新    docs/
API References         各 Developer   实时        代码注释
Performance Reports    各 Team Lead   周度更新    reports/
Meeting Minutes        PM            会后 24h    wiki/
Risk Register         PM            每周五      仪表板
Decision Log          决策者        实时        wiki/
```

### 代码注释与文档规范

```
【C/C++ 代码注释规范】(使用 Doxygen)

/**
 * @brief 函数简短描述 (一句话)
 * 
 * @details 详细描述 (如需要)
 *
 * @param param1 参数 1 的说明
 * @param param2 参数 2 的说明
 * 
 * @return 返回值说明
 * 
 * @note 注意事项 (如有)
 * @warning 警告信息 (如有)
 * 
 * @see related_function()
 * 
 * Example:
 * @code
 * int result = my_function(10, 20);
 * @endcode
 */

【Python 代码注释规范】(Google Style)

def schedule_flow(flow_id, priority, bandwidth):
    """
    为新流添加 TSN 调度规则.
    
    添加流的详细说明... (2-3 句)
    
    Args:
        flow_id: 流 ID (int)
        priority: 优先级 0-7 (int)
        bandwidth: 预留带宽 Mbps (float)
    
    Returns:
        bool: 是否成功 (True 成功, False 入场控制拒绝)
    
    Raises:
        ValueError: 如果优先级不在 0-7
        RuntimeError: 如果 taprio 命令执行失败
    
    Note:
        支持热更新 (无需重启调度器)
    
    Example:
        >>> ok = schedule_flow("flow_1", 7, 100.0)
        >>> assert ok
    """
```

### Wiki 知识库结构

```
Wiki (GitHub Wiki or Notion)

├─ 📘 Getting Started (快速入门)
│  ├─ Setup Development Environment
│  ├─ Build & Run First Demo
│  └─ Common Issues FAQ
│
├─ 🏗️ Architecture (架构)
│  ├─ System Overview
│  ├─ PTP Protocol Design
│  ├─ TSN Scheduling Algorithm
│  └─ Hardware Design
│
├─ 💻 Development (开发指南)
│  ├─ Git Workflow
│  ├─ Code Review Process
│  ├─ Testing Guidelines
│  ├─ Performance Profiling
│  └─ Debugging Tips
│
├─ 📊 Project Management (项目管理)
│  ├─ W2-W4 Milestone Tracking
│  ├─ Risk Register
│  ├─ Decision Log
│  ├─ Meeting Minutes Archive
│  └─ Team Contact Info
│
└─ 🔗 External Resources
   ├─ IEEE 1588 v2 Spec
   ├─ IEEE 802.1Qbv Spec
   └─ Related Papers & Tools
```

---

## 问题报告与反馈

### 问题报告流程 (GitHub Issues)

```
【优先级 P0 - 阻塞问题】

标题: [BLOCKER] 编译崩溃 - messages.c 头文件错误
标签: blocker, ptp, urgent
分配: Alice (PTP Lead)
优先级: P0

描述:
编译命令: cmake .. && make
错误消息: [粘贴完整错误]
发生时间: 2026-01-28 11:30
影响范围: PTP 开发完全阻塞
原因推测: messages.h 循环包含

复现步骤:
1. cd ptp-stack/build
2. cmake ..
3. make
4. [期望]:编译成功
   [实际]:ERROR: ...

建议修复: 检查 #include 依赖链

【优先级 P1 - 高优先问题】

标题: PTP 单元测试覆盖率下降至 38%
标签: tests, ptp, p1
分配: Test Engineer
优先级: P1

描述:
上周覆盖率: 42%
本周覆盖率: 38%
降幅: 4%

影响模块:
- messages.c: 42% → 35% (新增 SYNC 代码未覆盖)

原因分析:
新增的 SYNC 消息处理代码 (50 LOC) 还没有对应单元测试

建议修复: 添加 SYNC 消息的单元测试 (预计 2 小时)

【优先级 P2 - 中优先问题】

标题: TSN 性能基准低于目标
标签: performance, tsn, p2
分配: Bob (TSN Lead)

描述:
目标: <500µs
实测: 620µs (在 Ubuntu 18.04, taprio 不可用的情况下)

分析:
Linux 版本过旧, 不支持 taprio
建议: 在 Ubuntu 20.04+ 环境测试

下一步: 在支持 taprio 的环境重新测试

【优先级 P3 - 低优先问题】

标题: 文档中代码示例有拼写错误
标签: documentation, p3
分配: 任何人
优先级: P3

描述:
docs/PTP_ARCHITECTURE.md 第 42 行
拼写: "synchrounous" 应为 "synchronous"

建议修复: 更正拼写
```

### 反馈与建议渠道

```
【技术建议】(如何优化代码、改进设计)

发送方式: GitHub Discussions (category: Ideas)
标题: [Idea] PTP 性能优化: 使用环形缓冲代替队列
描述:
  当前的 SYNC 消息队列使用标准 malloc/free
  建议使用环形缓冲以减少内存碎片并提高性能
  预期性能提升: 15-20%

讨论: 所有人可评论和投票

【个人反馈】(关于文档、流程、工作环境)

发送方式: 私信 PM
内容例:
  "当前每日站会 15 分钟太紧张了,
   特别是当有问题需要讨论时。
   建议改为 20 分钟。"

PM 反馈: 收集所有意见,
          月度总结后讨论与改进
```

---

## 总结检查清单

### 团队成员检查清单

```
【每天】
☐ 10:00 参加每日站会
☐ 更新 Slack #daily-standup (10:30)
☐ 检查 GitHub Issues (新问题/指派)
☐ 回复 Slack 消息 (24h 内)

【每周】
☐ 周五 14:00 参加技术评审
☐ 提交周度进度报告 (如需要)
☐ 代码提交至少 1 次 PR
☐ 单元测试覆盖率维持或提升

【每月】
☐ 参加月度总结会议
☐ 更新个人开发进度
☐ 提供反馈与建议
```

### PM 检查清单

```
【每天】
☐ 10:00-10:15 主持每日站会
☐ 更新 Slack #daily-standup 摘要
☐ 审查 GitHub Issues (新建的)
☐ 检查是否有 P0 问题需要立即处理

【每周】
☐ 周五 14:00-16:00 主持技术评审
☐ 更新风险寄存器与决策日志
☐ 发布周度进度总结邮件
☐ 确认下周重点与行动项
☐ 根据监控仪表板评估健康度

【每两周】
☐ 高层进度汇报 (赞助商)
☐ 团队效率分析 (速度、质量)
☐ 资源分配评估 (是否需要调整)
```

---

**制定日期**: 2026-01-20  
**启动日期**: 2026-01-27 (W2)  
**维护者**: Project Manager  
**最后更新**: 2026-01-20
