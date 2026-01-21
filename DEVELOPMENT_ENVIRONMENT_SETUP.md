# 开发环境完整配置指南

**目标**: 各团队在 1月24日前完成环境配置  
**验证**: 1月25日环境就绪确认  
**启动**: 1月28日编码冲刺开始  

---

## 1. 全局开发环境配置

### 1.1 所有开发人员

#### 必装工具

```bash
# Git 配置 (所有人)
git config --global user.name "Your Name"
git config --global user.email "your.email@company.com"
git config --global core.autocrlf input
git config --global core.editor "vim"  # or your preferred editor

# SSH Key 生成 (首次)
ssh-keygen -t ed25519 -C "your.email@company.com"
# 将 ~/.ssh/id_ed25519.pub 添加到 GitHub SSH Keys

# 克隆仓库
cd d:\VScode专用
git clone https://github.com/OpenNeuro/OpenNeuro.git
cd OpenNeuro
git remote -v  # 验证远程仓库
```

#### 编辑器推荐配置

**VS Code 推荐插件**:
```
- C/C++ (Microsoft)
- CMake (twxs)
- CMake Tools (Microsoft)
- Python (Microsoft)
- Pylance
- Git Graph
- GitLens
- Markdown Preview Enhanced
- YAML (RedHat)
- Docker (Microsoft)
```

**安装**:
```bash
code --install-extension ms-vscode.cpptools
code --install-extension twxs.cmake
code --install-extension ms-vscode.cmake-tools
code --install-extension ms-python.python
code --install-extension ms-python.vscode-pylance
code --install-extension mhutchie.git-graph
code --install-extension eamodio.gitlens
code --install-extension shd101wyy.markdown-preview-enhanced
```

#### 创建工作目录结构

```bash
cd d:\VScode专用\OpenNeuro

# 创建分支
git checkout -b feature/environment-setup
git checkout -b feature/ptp-master
git checkout -b feature/tsn-scheduler
git checkout -b feature/zone-controller

# 创建本地目录
mkdir -p ptp-stack/src/{master,common,tests}
mkdir -p tsn-scheduler/src/{core,linux,tests}
mkdir -p zone-controller/hardware/kicad
mkdir -p zone-controller/docs
mkdir -p tests/{integration,performance}
```

---

## 2. PTP Team 开发环境

### 2.1 必装工具

```bash
# Windows
# 方案A: MSVC (推荐用于Windows开发)
# - Visual Studio 2022 Community (或 Build Tools)
# - 选择: C++ 开发工作负荷

# 方案B: MinGW-w64 (跨平台)
# 下载: https://www.mingw-w64.org/
# 添加到 PATH: C:\mingw64\bin

# 验证安装
gcc --version
g++ --version
cmake --version  # 需要 3.20+
```

#### CMake 安装 (如果未安装)

```bash
# 下载 CMake
# https://cmake.org/download/
# 选择 Windows x64 Installer
# 安装时选择 "Add CMake to system PATH"

# 验证
cmake --version
```

#### 单元测试框架安装

```bash
# Google Test (推荐)
cd d:\VScode专用\OpenNeuro\ptp-stack

# 创建 vcpkg 配置 (可选，自动管理依赖)
# 或手动下载 Google Test

# 使用 CMake 集成 (见后续 CMakeLists.txt)
```

### 2.2 编译环境验证

```bash
cd d:\VScode专用\OpenNeuro\ptp-stack
mkdir build
cd build

# Windows MSVC
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release

# 或 MinGW
cmake .. -G "Unix Makefiles"
make
```

### 2.3 PTP 代码框架

#### 项目结构
```
ptp-stack/
├── CMakeLists.txt              # 构建配置
├── src/
│   ├── CMakeLists.txt
│   ├── master/                 # PTP Master 实现
│   │   ├── messages.h          # 消息定义
│   │   ├── messages.c
│   │   ├── state_machine.h     # 状态机
│   │   ├── state_machine.c
│   │   ├── servo.h             # PI 伺服
│   │   ├── servo.c
│   │   ├── clock.h             # 时钟管理
│   │   └── clock.c
│   ├── common/                 # 公共代码
│   │   ├── types.h             # 类型定义
│   │   ├── utils.h
│   │   └── utils.c
│   └── ptp.h                   # 主公共 API
├── tests/
│   ├── CMakeLists.txt
│   ├── test_messages.cpp       # 消息单元测试
│   ├── test_state_machine.cpp
│   ├── test_servo.cpp
│   └── test_clock.cpp
├── docs/
│   ├── API.md                  # API 文档
│   └── DESIGN.md               # 设计文档
└── README.md
```

#### CMakeLists.txt 模板

```cmake
cmake_minimum_required(VERSION 3.20)
project(ptp-stack C)

# 编译选项
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")

# 调试符号
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")

# 源文件
file(GLOB_RECURSE SOURCES "src/**/*.c")
file(GLOB_RECURSE HEADERS "src/**/*.h")

# 主库
add_library(ptp_core STATIC ${SOURCES})
target_include_directories(ptp_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# 单元测试
enable_testing()
find_package(GTest REQUIRED)

file(GLOB TEST_SOURCES "tests/test_*.cpp")
foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME} ptp_core GTest::gtest GTest::gtest_main)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()

# 代码覆盖率 (可选)
if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
endif()
```

### 2.4 初始代码框架

#### src/common/types.h
```c
#ifndef PTP_TYPES_H
#define PTP_TYPES_H

#include <stdint.h>
#include <time.h>

/* PTP 时间戳 */
typedef struct {
    uint32_t seconds;
    uint32_t nanoseconds;
} ptp_timestamp_t;

/* 时钟信息 */
typedef struct {
    ptp_timestamp_t current_time;
    int64_t offset;          /* ns */
    int64_t frequency_offset; /* ppb */
} ptp_clock_info_t;

/* 消息类型 */
typedef enum {
    PTP_ANNOUNCE = 0x0B,
    PTP_SYNC = 0x00,
    PTP_FOLLOW_UP = 0x08,
    PTP_DELAY_REQ = 0x01,
    PTP_DELAY_RESP = 0x09,
} ptp_msg_type_t;

#endif /* PTP_TYPES_H */
```

#### src/master/messages.h
```c
#ifndef PTP_MESSAGES_H
#define PTP_MESSAGES_H

#include "../common/types.h"

/* PTP 消息处理 */
typedef struct {
    ptp_msg_type_t type;
    uint8_t* data;
    int length;
} ptp_message_t;

/* 初始化 */
int ptp_messages_init(void);

/* 发送消息 */
int ptp_send_announce(void);
int ptp_send_sync(void);
int ptp_send_follow_up(ptp_timestamp_t timestamp);

/* 处理消息 */
int ptp_handle_message(ptp_message_t* msg);

#endif /* PTP_MESSAGES_H */
```

---

## 3. TSN Team 开发环境

### 3.1 必装工具

```bash
# Linux 内核源码
sudo apt-get install linux-headers-$(uname -r)
sudo apt-get install linux-source

# Python 3.8+
python3 --version
pip3 install --upgrade pip

# 网络工具
sudo apt-get install ethtool iproute2 iperf3 netperf tcpdump

# 内核开发工具
sudo apt-get install build-essential gcc make
```

#### TSN 工具链

```bash
# Linux tc 工具 (通常已装)
tc --version

# taprio 驱动验证
sudo modinfo sch_taprio

# 可选：网络仿真工具
sudo apt-get install iproute2-doc

# Python 依赖
pip3 install numpy matplotlib scipy
```

### 3.2 TSN 代码框架

#### 项目结构
```
tsn-scheduler/
├── setup.py                    # Python 包配置
├── src/
│   ├── __init__.py
│   ├── tsn/
│   │   ├── __init__.py
│   │   ├── core/
│   │   │   ├── __init__.py
│   │   │   ├── queue.py        # 优先级队列
│   │   │   ├── scheduler.py    # 调度器
│   │   │   └── flow.py         # 流表管理
│   │   ├── linux/
│   │   │   ├── __init__.py
│   │   │   ├── tc_driver.py    # tc 命令包装
│   │   │   └── taprio.py       # taprio 驱动
│   │   └── utils/
│   │       ├── __init__.py
│   │       ├── logger.py
│   │       └── metrics.py
├── tests/
│   ├── __init__.py
│   ├── test_queue.py           # 单元测试
│   ├── test_scheduler.py
│   └── test_integration.py     # 集成测试
├── docs/
│   ├── API.md
│   └── DESIGN.md
├── examples/
│   └── basic_scheduling.py
└── README.md
```

#### setup.py 模板

```python
from setuptools import setup, find_packages

setup(
    name='tsn-scheduler',
    version='0.2.0',
    description='IEEE 802.1Qbv TSN Scheduler',
    author='Your Team',
    packages=find_packages(),
    python_requires='>=3.8',
    install_requires=[
        'numpy>=1.20',
        'pyyaml>=5.4',
    ],
    extras_require={
        'dev': ['pytest>=6.0', 'pytest-cov>=2.0'],
    },
)
```

### 3.3 初始代码框架

#### src/tsn/core/queue.py
```python
"""优先级队列实现"""

class PriorityQueue:
    """8 级优先级队列 (IEEE 802.1Q)"""
    
    def __init__(self, num_priorities=8):
        self.queues = [[] for _ in range(num_priorities)]
        self.num_priorities = num_priorities
    
    def enqueue(self, packet, priority=3):
        """入队"""
        if priority >= self.num_priorities:
            raise ValueError(f"Priority {priority} out of range")
        self.queues[priority].append(packet)
    
    def dequeue(self):
        """按优先级出队 (高优先级优先)"""
        for priority in range(self.num_priorities - 1, -1, -1):
            if self.queues[priority]:
                return self.queues[priority].pop(0)
        return None
    
    def get_stats(self):
        """获取队列统计"""
        return {
            'total': sum(len(q) for q in self.queues),
            'by_priority': [len(q) for q in self.queues],
        }
```

#### src/tsn/core/scheduler.py
```python
"""TSN 调度器"""

class TSNScheduler:
    """IEEE 802.1Qbv 时间感知调度器"""
    
    def __init__(self, cycle_time_us=1000):
        self.cycle_time = cycle_time_us / 1e6  # 转换为秒
        self.gate_states = [0] * 8  # 8 个优先级的门状态
        self.flow_table = {}
    
    def add_flow(self, flow_id, vlan, priority, bandwidth_mbps):
        """添加流"""
        self.flow_table[flow_id] = {
            'vlan': vlan,
            'priority': priority,
            'bandwidth': bandwidth_mbps,
        }
    
    def configure_gating(self, schedule):
        """配置门控计划"""
        # schedule: [(time_us, gate_state), ...]
        self.schedule = schedule
    
    def get_gate_state(self, time_us):
        """获取特定时刻的门状态"""
        # 实现门控算法
        pass
```

---

## 4. 硬件 Team 开发环境

### 4.1 KiCad 安装与配置

```bash
# Windows
# 下载: https://kicad.org/download/
# 选择最新稳定版 (7.0+)
# 安装时选择:
#   - ✓ KiCad
#   - ✓ Libraries
#   - ✓ Documentation

# 验证安装
kicad --version
```

### 4.2 KiCad 项目初始化

```bash
cd d:\VScode专用\OpenNeuro\zone-controller\hardware\kicad

# 创建新项目
# File → New Project → zone-controller

# 项目文件结构将创建为:
zone-controller/
├── zone-controller.kicad_pro     # 项目文件
├── zone-controller.kicad_sch     # 原理图
├── zone-controller.kicad_pcb     # PCB 布局
├── zone-controller-cache.lib      # 符号缓存
└── sym-lib-table                  # 符号库表
```

### 4.3 KiCad 库配置

```bash
# 添加第三方库 (可选)
# 推荐库:
# - 工业标准库: https://github.com/KiCad/kicad-symbols
# - 足迹库: https://github.com/KiCad/kicad-footprints

# 在 KiCad 中配置:
# Preferences → Manage Symbol Libraries
# 添加: ${KICAD6_SYMBOL_DIR}/Symbol
```

### 4.4 硬件设计框架

#### 硬件项目结构
```
zone-controller/
├── hardware/
│   ├── kicad/
│   │   ├── zone-controller.kicad_pro
│   │   ├── zone-controller.kicad_sch
│   │   ├── zone-controller.kicad_pcb
│   │   ├── datasheets/              # 芯片数据手册
│   │   │   ├── RK3588.pdf
│   │   │   ├── RJ45.pdf
│   │   │   └── ...
│   │   └── libraries/               # 自定义库
│   │       ├── symbols.kicad_sym
│   │       └── footprints.kicad_mod
│   ├── gerber/                      # 生产文件 (后期生成)
│   └── docs/
│       ├── design_rules.md          # PCB 设计规则
│       ├── BOM.csv                  # 物料清单
│       └── assembly.txt             # 装配说明
└── firmware/                        # 硬件相关固件
    └── bootloader/
```

### 4.5 硬件设计检查清单

```
设计验证清单:
□ 原理图符号库已配置
□ 足迹库已配置
□ 参考原理图 (SCHEMATIC_DESIGN_GUIDE.md)
□ 创建第一个原理图页面 (电源系统)
□ 添加主要芯片:
  □ RK3588 (处理器)
  □ 电源管理芯片
  □ 网络接口芯片
□ 添加被动器件库存
□ 检查电源树
□ 验证信号完整性
```

---

## 5. QA Team 测试环境

### 5.1 必装工具

```bash
# Python 测试框架
pip install pytest pytest-cov

# C++ 测试框架
# Google Test (包含在 PTP 的 CMakeLists.txt)

# 代码覆盖率工具
pip install coverage
# Windows: 安装 OpenCppCoverage (可选)

# CI/CD (GitHub Actions 自动化)
# 无需本地安装，通过 .github/workflows 配置
```

### 5.2 测试项目结构

```
tests/
├── unit/
│   ├── test_ptp_messages.cpp
│   ├── test_ptp_servo.cpp
│   ├── test_tsn_queue.py
│   └── test_tsn_scheduler.py
├── integration/
│   ├── test_ptp_tsn_integration.py
│   └── test_hardware_interfaces.py
├── performance/
│   ├── perf_ptp_latency.cpp
│   ├── perf_tsn_throughput.py
│   └── perf_hardware_timing.cpp
├── conftest.py                  # pytest 配置
├── pytest.ini                   # pytest 配置文件
└── README.md                    # 测试文档
```

### 5.3 pytest 配置

#### tests/pytest.ini
```ini
[pytest]
testpaths = tests
python_files = test_*.py
python_classes = Test*
python_functions = test_*
addopts = -v --tb=short --cov=src --cov-report=html
markers =
    slow: marks tests as slow (deselect with '-m "not slow"')
    integration: marks tests as integration tests
```

#### tests/conftest.py
```python
"""pytest 配置和共享 fixtures"""

import pytest

@pytest.fixture
def sample_packet():
    """测试数据包"""
    return b'\x00' * 64

@pytest.fixture
def mock_queue():
    """模拟队列"""
    from unittest.mock import Mock
    return Mock()
```

### 5.4 CI/CD 配置

#### .github/workflows/test-ptp.yml
```yaml
name: PTP Tests

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest
    strategy:
      matrix:
        compiler: [gcc, clang]
    
    steps:
    - uses: actions/checkout@v2
    
    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y build-essential cmake
        
    - name: Build PTP Stack
      run: |
        cd ptp-stack
        mkdir build && cd build
        cmake ..
        cmake --build .
    
    - name: Run tests
      run: |
        cd ptp-stack/build
        ctest --output-on-failure
```

#### .github/workflows/test-tsn.yml
```yaml
name: TSN Tests

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v2
    
    - name: Set up Python
      uses: actions/setup-python@v2
      with:
        python-version: '3.9'
    
    - name: Install dependencies
      run: |
        pip install -r requirements-dev.txt
    
    - name: Run tests
      run: |
        cd tsn-scheduler
        pytest --cov=src tests/
    
    - name: Upload coverage
      uses: codecov/codecov-action@v2
```

---

## 6. 环境验证清单

### 6.1 每个开发人员检查清单

#### PTP Team
- [ ] Git 配置完成
- [ ] C/C++ 编译器安装 (gcc/clang)
- [ ] CMake 3.20+ 安装
- [ ] VS Code + C++ 插件安装
- [ ] ptp-stack 项目编译成功
- [ ] 单元测试框架可用
- [ ] 代码编辑器配置完成

#### TSN Team
- [ ] Git 配置完成
- [ ] Python 3.8+ 安装
- [ ] Linux 环境 (WSL2 或 Linux 虚拟机)
- [ ] tc/taprio 工具可用
- [ ] 网络测试工具安装
- [ ] VS Code + Python 插件安装
- [ ] tsn-scheduler 项目结构完成

#### 硬件 Team
- [ ] Git 配置完成
- [ ] KiCad 7.0+ 安装
- [ ] KiCad 库配置完成
- [ ] 项目文件创建成功
- [ ] 参考设计文档已读
- [ ] 设计工具链验证完成

#### QA Team
- [ ] Git 配置完成
- [ ] Python 3.8+ + pytest 安装
- [ ] 代码覆盖率工具安装
- [ ] CI/CD 工作流配置
- [ ] GitHub Actions 测试通过
- [ ] 本地测试运行成功

### 6.2 环境验证命令

```bash
# 全员
git --version
git config --list | grep user

# PTP Team
gcc --version
cmake --version
cd ptp-stack && mkdir build && cd build && cmake .. && cmake --build .

# TSN Team
python3 --version
tc --version
pip list | grep -E "numpy|pyyaml"

# 硬件 Team
kicad --version

# QA Team
pytest --version
coverage --version
```

### 6.3 最终验证

```bash
# 1月24日下午
# PM 运行环境验证脚本
cd d:\VScode专用\OpenNeuro
python scripts/verify_environment.py

# 输出应显示:
# ✓ PTP Team: 3/3 环境就绪
# ✓ TSN Team: 3/3 环境就绪
# ✓ 硬件 Team: 3/3 环境就绪
# ✓ QA Team: 3/3 环境就绪
# 整体: 100% 就绪
```

---

## 7. 常见问题解决

### Windows 编译问题

**问题**: cmake 找不到编译器
```bash
# 解决方案
cmake .. -G "Visual Studio 17 2022" -A x64

# 或使用 MinGW
cmake .. -G "Unix Makefiles" -DCMAKE_C_COMPILER=gcc
```

### Linux 权限问题

```bash
# 网络工具需要 root
sudo modprobe sch_taprio
sudo tc qdisc add ...

# 或添加用户到 netdev 组
sudo usermod -a -G netdev $USER
```

### KiCad 库问题

```bash
# 库加载失败
# 解决: Preferences → Manage Libraries → 重新添加库路径
# 或重新安装 KiCad (选中 Libraries)
```

---

## 8. 启动检查清单 (1月24日)

**PM 检查清单** (下午 17:00):

- [ ] 所有团队成员报告环境就绪
- [ ] Git 分支创建确认
- [ ] 代码框架初始化完成
- [ ] CI/CD 流水线测试通过
- [ ] 所有工具链验证通过
- [ ] 项目编译/运行成功

**发送给全队的清单** (下午 18:00):

```
✓ 开发环境: 100% 就绪
✓ 代码框架: 初始化完成
✓ CI/CD: 配置完成
✓ 团队分工: 确认完成

【下周一启动会议】
时间: 1月27日 14:00-16:00
地点: [会议室或在线链接]
议程:
  14:00-14:10 欢迎 & 执行说明
  14:10-14:25 PTP Team 任务分解
  14:25-14:40 TSN Team 任务分解
  14:40-14:55 硬件 Team 任务分解
  14:55-15:15 Q&A & 风险讨论
  15:15-15:30 工作流演示
  15:30-16:00 自由讨论

【下周二编码启动】
时间: 1月28日 09:00
正式开始: W2 编码冲刺
```

---

**环境配置完成后准备进入 CODE_FRAMEWORK_INITIALIZATION 阶段！**
