# 开发环境快速参考

> 针对OpenNeuro Stage 2各团队的快速操作指南  
> **版本**: 1.0  
> **最后更新**: 2026-01-20  

---

## 📋 快速检查清单 (1min)

```
当前环境检查:
✓ Git 已安装          → git --version
✓ 开发工具已安装       → 根据团队选择
✓ IDE 已配置          → VS Code + 扩展
✓ 项目目录已创建       → d:\VScode专用\OpenNeuro\
✓ 代码已克隆           → git clone ...
```

---

## 🚀 各团队5分钟启动

### PTP Team 启动

```bash
# 1. 进入项目目录
cd d:\VScode专用\OpenNeuro\ptp-stack

# 2. 创建编译目录
mkdir build && cd build

# 3. 编译项目
cmake ..
cmake --build . --config Release

# 4. 运行测试
ctest --output-on-failure

# 预期输出:
# ✓ 编译成功 (0 errors)
# ✓ 所有测试通过 (3/3 passed)
```

**验证成功条件**:
- [ ] 编译无错误
- [ ] 所有测试通过
- [ ] 可执行文件生成

### TSN Team 启动

```bash
# 1. 进入项目目录
cd d:\VScode专用\OpenNeuro\tsn-scheduler

# 2. 安装依赖
pip install -e ".[dev]"

# 3. 运行测试
pytest tests/unit -v

# 4. 检查覆盖率
pytest tests/unit --cov=src --cov-report=html

# 预期输出:
# ✓ 所有测试通过 (5/5 passed)
# ✓ 覆盖率 >= 80%
```

**验证成功条件**:
- [ ] 依赖安装成功
- [ ] 所有测试通过
- [ ] 覆盖率达标

### Hardware Team 启动

```bash
# 1. 打开 KiCad 7.0+
File → Open Project

# 2. 打开项目文件
zone-controller/hardware/kicad/zone-controller.kicad_pro

# 3. 验证库配置
Preferences → Manage Symbol Libraries → 检查路径

# 4. 新建原理图页面
File → New Sheet → Sheet 1 - Power

# 预期界面:
# ✓ KiCad 主窗口打开
# ✓ 项目树可见
# ✓ 库已加载
```

**验证成功条件**:
- [ ] KiCad 7.0+ 已安装
- [ ] 项目成功打开
- [ ] 库已正确配置

### QA Team 启动

```bash
# 1. 进入项目目录
cd d:\VScode专用\OpenNeuro

# 2. 安装测试工具
pip install pytest pytest-cov coverage

# 3. 验证 CI/CD 配置
ls -la .github/workflows/ci-*.yml

# 4. 运行测试发现
pytest --collect-only

# 预期输出:
# ✓ 所有测试文件被发现
# ✓ CI/CD 工作流可见
```

**验证成功条件**:
- [ ] pytest 已安装
- [ ] 测试工具可用
- [ ] CI/CD 配置就绪

---

## 📁 项目目录结构一览

```
d:\VScode专用\OpenNeuro\
├── ptp-stack/                          # PTP 精密时钟
│   ├── src/
│   │   ├── master/                     # Master 实现
│   │   ├── common/                     # 公共模块
│   │   └── protocol/                   # 协议定义
│   ├── tests/                          # 单元测试
│   ├── CMakeLists.txt                  # 编译配置
│   └── build/                          # 编译输出
│
├── tsn-scheduler/                      # TSN 网络调度
│   ├── src/tsn/
│   │   ├── core/                       # 核心队列、调度器
│   │   ├── linux/                      # Linux 实现
│   │   └── utils/                      # 工具函数
│   ├── tests/                          # 单元测试
│   ├── setup.py                        # 包配置
│   └── README.md                       # 文档
│
├── zone-controller/                    # 硬件控制器
│   ├── hardware/kicad/                 # KiCad 项目
│   ├── datasheets/                     # 芯片数据手册
│   ├── libraries/                      # 自定义库
│   └── docs/                           # 设计文档
│
├── .github/workflows/                  # CI/CD 工作流
│   ├── ci-ptp.yml                      # PTP 流水线
│   ├── ci-tsn.yml                      # TSN 流水线
│   └── ci-hardware.yml                 # 硬件检查
│
├── DEVELOPMENT_ENVIRONMENT_SETUP.md    # 环境配置指南
├── CODE_FRAMEWORK_INITIALIZATION.md    # 框架初始化指南
└── QUICK_REFERENCE.md                  # 本文件
```

---

## 🔧 常用命令速查表

### Git 工作流

```bash
# 查看当前分支
git branch -a

# 创建功能分支
git checkout -b feature/component-name

# 提交代码
git add .
git commit -m "feat: 描述功能"
git push origin feature/component-name

# 创建 Pull Request
# → GitHub 网页版操作

# 合并到 develop
git checkout develop
git pull origin develop
git merge feature/component-name
git push origin develop
```

### PTP 编译命令

```bash
# 完整编译流程
cd ptp-stack/build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel 4

# 运行单个测试
ctest -R test_name -VV

# 生成覆盖率报告
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build . --target coverage

# 清理编译目录
rm -rf build && mkdir build
```

### TSN 测试命令

```bash
# 运行所有测试
cd tsn-scheduler
pytest tests/ -v

# 运行特定测试
pytest tests/unit/test_queue.py -v

# 查看覆盖率
pytest tests/ --cov=src --cov-report=term-missing

# 生成 HTML 报告
pytest tests/ --cov=src --cov-report=html
# 输出: htmlcov/index.html
```

### 硬件设计命令

```bash
# 打开 KiCad 项目
kicad zone-controller/hardware/kicad/zone-controller.kicad_pro

# 导出原理图为 PDF
# File → Export → Plot → PDF

# 生成 BOM
# Tools → Generate BOM

# 设计规则检查
# Tools → Design Rule Checker (DRC)
```

---

## 🐛 常见问题快速解答

### Q: CMake 编译失败 "command not found"

**A**: Windows 需要安装 Visual Studio Build Tools 或 MinGW

```bash
# 检查 CMake 安装
cmake --version

# 检查编译器
gcc --version    # or cl (MSVC)

# 重新配置
cd ptp-stack/build
cmake .. -G "Unix Makefiles"  # Linux/macOS
cmake .. -G "Visual Studio 16 2019"  # Windows
```

### Q: pytest 找不到模块

**A**: Python 路径配置问题

```bash
# 检查包安装
pip list | grep tsn

# 重新安装
cd tsn-scheduler
pip install -e .

# 验证导入
python -c "import tsn; print(tsn.__version__)"
```

### Q: KiCad 库文件找不到

**A**: 库路径配置错误

```
KiCad → Preferences → Manage Symbol Libraries
检查路径是否正确指向:
- zone-controller/libraries/symbols/
- zone-controller/libraries/footprints/
```

### Q: GitHub Actions 工作流失败

**A**: 检查工作流文件和触发条件

```bash
# 验证 YAML 语法
# https://www.yamllint.com/

# 查看详细错误
GitHub → Actions → 查看失败的 workflow

# 常见原因:
# 1. 缩进错误
# 2. 依赖版本过旧
# 3. 路径不存在
```

---

## 📊 团队周期检查

### 每日 (10:00-10:15) - Daily Standup

```
报告内容:
✓ 昨日完成: _______________
✓ 今日计划: _______________
✓ 遇到问题: _______________

格式:
[PTP] completed X, working on Y, blocked by Z
[TSN] completed X, working on Y, no blockers
[HW]  completed X, working on Y, escalated to PM
[QA]  completed X, working on Y, all green
```

### 每周五 (14:00-16:00) - Weekly Review

```
检查项目:
✓ 周目标完成度: ___%
✓ 代码覆盖率: ___%
✓ 未解决问题数: __
✓ 计划变更数: __

提交物:
✓ 代码审查: 2x Approve
✓ 测试通过: 100%
✓ CI/CD 绿灯: ✓
```

### 月度 - Milestone Review

```
关键指标:
✓ LOC (新增代码行数)
✓ 测试覆盖率
✓ Bug 密度 (Bug/KLOC)
✓ 代码审查时间
✓ CI/CD 通过率
```

---

## 🎯 W2 里程碑目标 (1/28-2/9)

### PTP Team 目标

```
周一-周二: 实现 ANNOUNCE 消息 (50 LOC)
周三-周五: 完成测试 + 代码审查
验收标准:
✓ ANNOUNCE 消息格式正确
✓ 单元测试覆盖率 >= 80%
✓ 集成测试通过
✓ 代码审查通过

提交:
git commit -m "feat(ptp): implement ANNOUNCE message handling"
```

### TSN Team 目标

```
周一-周二: 实现优先级队列 (70 LOC)
周三-周四: 实现调度器基类
周五: 集成测试 + 代码审查
验收标准:
✓ 支持 8 个优先级 (IEEE 802.1Q)
✓ FIFO 模式正确
✓ 单元测试覆盖率 >= 85%
✓ 性能基准达成

提交:
git commit -m "feat(tsn): add priority queue with IEEE 802.1Q support"
```

### Hardware Team 目标

```
周一-周三: 电源系统原理图 (50% 完成)
周四: 设计规则检查 (DRC)
周五: 文档 + 审查
验收标准:
✓ 电源树完整
✓ 纹波 < 100mV
✓ DRC 无错误
✓ BOM 更新

提交:
git commit -m "feat(hw): complete power distribution system schematic"
```

### QA Team 目标

```
持续:
✓ 维护 CI/CD 流水线
✓ 监控代码覆盖率
✓ 运行性能基准测试
✓ 生成周报

输出:
✓ 每日 CI/CD 报告
✓ 周覆盖率报告
✓ 性能趋势图
```

---

## 📞 技术支持联系

| 问题类型 | 负责人 | 渠道 |
|---------|------|------|
| PTP 框架 | Lead-PTP | #dev-ptp Slack |
| TSN 框架 | Lead-TSN | #dev-tsn Slack |
| 硬件设计 | Lead-HW | #dev-hardware Slack |
| CI/CD 问题 | Lead-QA | #dev-qa Slack |
| 整体协调 | PM | @Channel |

---

## 🔗 关键文档链接

- 📘 [完整环境配置指南](DEVELOPMENT_ENVIRONMENT_SETUP.md)
- 📘 [代码框架初始化指南](CODE_FRAMEWORK_INITIALIZATION.md)
- 📘 [执行启动检查清单](EXECUTION_STARTUP_CHECKLIST.md)
- 📘 [W2-W4 执行计划](W3_W4_EXECUTION_PLAN.md)
- 📘 [监控仪表板](STAGE2_MONITORING_DASHBOARD.md)
- 📘 [风险决策框架](RISK_DECISION_FRAMEWORK.md)

---

## ✅ 最终检查清单

在开始 W2 编码前，确保:

```
□ 开发环境
  □ 所需工具已安装
  □ 编译/测试验证通过
  □ IDE 配置完成

□ 代码框架
  □ 项目目录结构创建
  □ 初始代码文件就位
  □ 分支创建完成

□ CI/CD 流水线
  □ GitHub Actions 配置
  □ 工作流首次运行通过
  □ 覆盖率工具就绪

□ 文档和知识
  □ 熟悉快速参考指南
  □ 了解团队流程
  □ 知道技术支持渠道

□ 团队协调
  □ 加入 Slack 频道
  □ 了解日站会时间
  □ 准备 W2 目标报告
```

---

**准备就绪? 让我们在 1月28日开始编码冲刺! 🚀**

---

*最后更新: 2026-01-20*  
*下次审查: 2026-01-27 (W2 启动会议)*
