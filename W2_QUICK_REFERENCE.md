# W2 快速参考卡 (Quick Reference)

**打印此文档用于快速查阅！**

---

## 📑 文档导航 (打开文档的快速命令)

```bash
# W2 工作计划
open W2_TECHNICAL_STARTUP.md

# PTP 相关
open docs/PTP_ARCHITECTURE.md              # W1 架构设计
open docs/PTP_MASTER_IMPLEMENTATION.md     # W2 实现设计 ⭐

# TSN 相关
open docs/TSN_REQUIREMENTS.md              # W1 需求规格
open docs/TSN_ALGORITHM_DESIGN.md          # W2 算法详设 ⭐

# 硬件相关
open hardware/DESIGN_SPECIFICATION.md      # W1 硬件设计
open hardware/SCHEMATIC_DESIGN_GUIDE.md    # W2 原理图指南 ⭐

# 项目总览
open README.md                             # 项目主文档
open W1_COMPLETION_REPORT.md               # W1 完成报告
```

---

## 🎯 本周关键数字

| 指标 | 目标 | 备注 |
|------|------|------|
| PTP 代码行数 | 200+ | messages/bmc/clock |
| TSN 代码行数 | 220+ | queue/scheduler |
| 测试覆盖率 | 40%+ | 两个项目都要求 |
| 硬件原理图 | 50% | 电源完整，其他初稿 |
| 评审通过 | 3/3 | PTP/TSN/HW 都要评审通过 |

---

## 📅 本周日程 (速查表)

```
周一  10:00  日常站会        (30 min)
      14:00  设计评审        (2 hrs)
      16:00  结束

周二  09:00  集中编码        (3 hrs)
      14:00  同步+审查       (3 hrs)

周三  09:00  集中编码        (3 hrs)
      14:00  同步+审查       (3 hrs)

周四  09:00  集中编码        (3 hrs)
      14:00  同步+审查       (3 hrs)

周五  10:00  周度技术站会    (2 hrs)
      14:00  代码评审        (1 hr)
      15:00  结束
```

---

## 🔧 快速启动 (5 分钟)

### 1. 克隆代码

```bash
cd ~/projects
git clone https://github.com/OpenNeuro/OpenNeuro.git
cd OpenNeuro
git checkout develop
```

### 2. 创建开发分支

```bash
# PTP Team
git checkout -b feature/ptp-master-messages

# TSN Team
git checkout -b feature/tsn-queue-scheduler

# Hardware Team
git checkout -b feature/hardware-schematic
```

### 3. 构建验证

```bash
# PTP
cd ptp-stack && mkdir build && cd build && cmake .. && make

# TSN
cd ../../tsn-scheduler && mkdir build && cd build && cmake .. && make

# Hardware (KiCad)
cd ../../zone-controller/hardware/kicad
kicad zone-controller.pro
```

---

## 👥 团队任务分配

### PTP Team (200+ LOC)
```
MESSAGES  (150 LOC)  ├─ ANNOUNCE 消息构造
                     ├─ SYNC + FOLLOW_UP
                     ├─ DELAY_REQ/RESP
                     └─ 单元测试

BMC       (100 LOC)  ├─ Grandmaster 比较
                     ├─ 状态转移决策
                     └─ 单元测试

CLOCK     (80 LOC)   ├─ 时间维护
                     ├─ 硬件计数器
                     └─ 单元测试
```

### TSN Team (220+ LOC)
```
QUEUE     (120 LOC)  ├─ 优先级队列
                     ├─ enqueue/dequeue
                     └─ 单元测试

SCHEDULER (150 LOC)  ├─ 门控算法
                     ├─ 流量分类
                     └─ 单元测试

LINUX     (80 LOC)   ├─ taprio 框架
                     ├─ API 设计
                     └─ 初步实现
```

### Hardware Team
```
KICAD               ├─ 项目初始化
                    └─ 符号库导入

POWER               ├─ MP2107 配置
                    ├─ 输入保护
                    └─ 输出电容

NETWORK             ├─ RTL8211FD
                    ├─ RJ45
                    └─ EMI 过滤
```

---

## 📊 成功标准 (Definition of Done)

### 每日 (Daily)

```
☐ 代码能编译 (无警告)
☐ 本地单元测试通过
☐ 提交清晰的 commit message
☐ 更新相关文档
```

### 周度 (Weekly)

```
☐ Pull Request 已提交
☐ 代码审查已完成
☐ 单元测试覆盖率 > 40%
☐ 周度进度报告已提交
☐ 风险预警已识别
```

---

## 🐛 常见问题快速查阅

### Q: 如何运行单元测试?
```bash
cd [project]/build
./test_[project] --verbose
```

### Q: 如何提交 PR?
```bash
git push origin feature/your-task
# 在 GitHub 创建 Pull Request
# 填写模板，链接相关 Issue
# 等待代码审查与 CI 检查
```

### Q: 代码风格规范?
- 命名: `ptp_*`, `tsn_*` 前缀
- 注释: 函数必须有文档注释
- 格式: 4 空格缩进，`clang-format` 检查
- 检查: `make format` (自动格式化)

### Q: 如何报告阻塞问题?
- Slack: #w2-blockers 频道 (紧急)
- GitHub: Issue 标签 `blocked` (普通)
- 邮件: team@openneuro.ai (特殊情况)

---

## 📞 联系方式

```
CTO/Lead Dev:        slack: @lead-dev
TSN Lead:            slack: @tsn-lead
Hardware Lead:       slack: @hw-lead
项目经理:            slack: @pm

技术讨论群:         #technology-discussion
日常沟通:           #general
紧急阻塞:           #w2-blockers
代码审查:           GitHub PR Comments
```

---

## 🚀 下周 (W3) 预告

```
目标:     完整的 SYNC/DELAY 流程 + 硬件原理图 75%

PTP:      ├─ SYNC 消息完整流程
          ├─ DELAY_REQ/RESP 处理
          └─ 集成测试启动

TSN:      ├─ Linux 集成完成
          ├─ 带宽控制实现
          └─ 性能基准测试

Hardware: ├─ 网络接口原理图完成
          ├─ PTP 时间戳设计
          └─ PCB 初步布局
```

---

## 📚 推荐阅读 (今晚)

1. **W2_TECHNICAL_STARTUP.md** (30 min)
   - 本周完整工作计划与会议议程

2. **docs/PTP_MASTER_IMPLEMENTATION.md** (45 min)
   - PTP Master 完整消息处理设计

3. **docs/TSN_ALGORITHM_DESIGN.md** (45 min)
   - TSN 时间感知调度详细算法

4. **hardware/SCHEMATIC_DESIGN_GUIDE.md** (30 min)
   - 原理图设计规范与 KiCad 工作流

---

## ⚡ 紧急操作清单

**如果你今晚只有 5 分钟:**
1. 打开 W2_TECHNICAL_STARTUP.md
2. 找到你的任务分配
3. 克隆代码仓库
4. 创建你的开发分支

**如果你有 30 分钟:**
1. 审查你的任务相关文档 (PTP/TSN/HW)
2. 搭建开发环境
3. 运行 Hello World 构建
4. 创建 GitHub Issue 列表

**如果你有 2 小时:**
1. 完整审读所有 4 份 W2 文档
2. 搭建完整开发环境
3. 理解架构与算法
4. 准备代码框架
5. 参加 1月27日的设计评审

---

**最后更新**: 2026-01-20  
**下一次更新**: 2026-01-27 (周一)  
**打印日期**: ___________  
**签字**: ___________
