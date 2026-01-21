# 代码框架初始化指南

**目标**: 1月24-25日完成所有代码框架初始化  
**验证**: 1月26日 PM 检查确认  
**启动**: 1月28日编码冲刺  

---

## 1. 快速初始化脚本

### 1.1 一键初始化 (Windows PowerShell)

**文件**: `scripts/setup-frameworks.ps1`

```powershell
# 项目初始化脚本
# 用法: .\scripts\setup-frameworks.ps1

param(
    [string]$Mode = "all"  # all, ptp, tsn, hardware
)

$ErrorActionPreference = "Stop"
$ScriptPath = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectRoot = Split-Path -Parent $ScriptPath

Write-Host "════════════════════════════════════════════" -ForegroundColor Cyan
Write-Host "  OpenNeuro 代码框架初始化" -ForegroundColor Green
Write-Host "════════════════════════════════════════════" -ForegroundColor Cyan

# 检查 Git
function Check-Git {
    try {
        $version = git --version
        Write-Host "✓ Git: $version" -ForegroundColor Green
    }
    catch {
        Write-Host "✗ Git 未安装" -ForegroundColor Red
        exit 1
    }
}

# 创建 PTP 框架
function Initialize-PTP {
    Write-Host "`n📦 初始化 PTP Stack..." -ForegroundColor Yellow
    
    $PtpDir = "$ProjectRoot\ptp-stack"
    
    # 创建目录
    @(
        "src\master",
        "src\common",
        "tests",
        "docs"
    ) | ForEach-Object {
        $path = Join-Path $PtpDir $_
        if (!(Test-Path $path)) {
            New-Item -ItemType Directory -Path $path -Force | Out-Null
            Write-Host "  ✓ 创建目录: $_"
        }
    }
    
    # 创建初始文件
    Write-Host "  ✓ 创建初始代码文件..."
    # ... 代码文件内容由下面的具体段落提供
}

# 创建 TSN 框架
function Initialize-TSN {
    Write-Host "`n📦 初始化 TSN Scheduler..." -ForegroundColor Yellow
    
    $TsnDir = "$ProjectRoot\tsn-scheduler"
    
    # 创建目录
    @(
        "src\tsn\core",
        "src\tsn\linux",
        "src\tsn\utils",
        "tests",
        "docs"
    ) | ForEach-Object {
        $path = Join-Path $TsnDir $_
        if (!(Test-Path $path)) {
            New-Item -ItemType Directory -Path $path -Force | Out-Null
            Write-Host "  ✓ 创建目录: $_"
        }
    }
}

# 创建硬件框架
function Initialize-Hardware {
    Write-Host "`n📦 初始化 Zone Controller..." -ForegroundColor Yellow
    
    $HwDir = "$ProjectRoot\zone-controller"
    
    @(
        "hardware\kicad",
        "hardware\datasheets",
        "hardware\libraries",
        "docs"
    ) | ForEach-Object {
        $path = Join-Path $HwDir $_
        if (!(Test-Path $path)) {
            New-Item -ItemType Directory -Path $path -Force | Out-Null
            Write-Host "  ✓ 创建目录: $_"
        }
    }
}

# 主程序
Check-Git

if ($Mode -eq "all" -or $Mode -eq "ptp") { Initialize-PTP }
if ($Mode -eq "all" -or $Mode -eq "tsn") { Initialize-TSN }
if ($Mode -eq "all" -or $Mode -eq "hardware") { Initialize-Hardware }

Write-Host "`n✅ 框架初始化完成！" -ForegroundColor Green
Write-Host "下一步: 提交代码到 Git" -ForegroundColor Cyan
```

### 1.2 Git 初始化命令

```bash
cd d:\VScode专用\OpenNeuro

# 创建功能分支
git checkout develop
git pull origin develop

# 创建本地分支
git checkout -b feature/ptp-master-w2
git checkout -b feature/tsn-scheduler-w2
git checkout -b feature/zone-controller-w2

# 验证分支
git branch -a
```

---

## 2. PTP Stack 框架初始化

### 2.1 完整目录结构创建

```bash
cd ptp-stack

# 自动创建
mkdir -p src/{master,common,protocol,platform,tests}
mkdir -p tests/{unit,integration}
mkdir -p docs/{design,api,examples}
mkdir -p build
mkdir -p third_party/gtest
```

### 2.2 CMakeLists.txt (完整版本)

**文件**: `ptp-stack/CMakeLists.txt`

```cmake
cmake_minimum_required(VERSION 3.20)
project(ptp-stack C CXX)

# ============================================================================
# 编译配置
# ============================================================================

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 编译选项
if(MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4 /WX")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -fPIC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -fPIC")
endif()

# 调试和发布配置
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# ============================================================================
# 源文件
# ============================================================================

# 公共源文件
file(GLOB COMMON_SOURCES "src/common/*.c")

# Master 源文件
file(GLOB MASTER_SOURCES "src/master/*.c")

# 协议源文件
file(GLOB PROTOCOL_SOURCES "src/protocol/*.c")

# 平台适配源文件
set(PLATFORM_SOURCES "src/platform/platform.c")
if(UNIX)
    set(PLATFORM_SOURCES ${PLATFORM_SOURCES} "src/platform/linux.c")
elseif(WIN32)
    set(PLATFORM_SOURCES ${PLATFORM_SOURCES} "src/platform/windows.c")
endif()

# 合并所有源文件
set(PTP_SOURCES
    ${COMMON_SOURCES}
    ${MASTER_SOURCES}
    ${PROTOCOL_SOURCES}
    ${PLATFORM_SOURCES}
)

# ============================================================================
# 库构建
# ============================================================================

# 静态库
add_library(ptp_core STATIC ${PTP_SOURCES})
target_include_directories(ptp_core PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 动态库 (可选)
add_library(ptp_shared SHARED ${PTP_SOURCES})
target_include_directories(ptp_shared PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# ============================================================================
# 单元测试
# ============================================================================

enable_testing()

# Google Test 查找或下载
find_package(GTest QUIET)
if(NOT GTest_FOUND)
    message(STATUS "GTest 未找到，将尝试下载...")
    include(FetchContent)
    FetchContent_Declare(googletest
        URL https://github.com/google/googletest/archive/release-1.12.0.zip
    )
    FetchContent_MakeAvailable(googletest)
endif()

# 单元测试
file(GLOB TEST_SOURCES "tests/unit/test_*.cpp")
foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME} ptp_core gtest gtest_main)
    target_include_directories(${TEST_NAME} PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    )
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME} 
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endforeach()

# ============================================================================
# 代码覆盖率
# ============================================================================

if(CMAKE_BUILD_TYPE MATCHES Debug AND NOT MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
    
    # 添加覆盖率报告目标
    add_custom_target(coverage
        COMMAND ${CMAKE_MAKE_PROGRAM} test
        COMMAND gcovr --root ${CMAKE_CURRENT_SOURCE_DIR} 
                      --filter "src/" 
                      --exclude "tests/"
                      -r ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()

# ============================================================================
# 安装目标
# ============================================================================

install(TARGETS ptp_core ptp_shared
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY src/
    DESTINATION include/ptp
    FILES_MATCHING PATTERN "*.h"
)

# ============================================================================
# 信息输出
# ============================================================================

message(STATUS "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
message(STATUS "PTP Stack 编译配置")
message(STATUS "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C compiler: ${CMAKE_C_COMPILER}")
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "Source files: ${PTP_SOURCES}")
message(STATUS "Test files: ${TEST_SOURCES}")
message(STATUS "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
```

### 2.3 初始头文件

**文件**: `ptp-stack/src/ptp.h`

```c
#ifndef PTP_H
#define PTP_H

#include <stdint.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * PTP Stack 公共 API
 * ============================================================================ */

/* 初始化 PTP 栈 */
int ptp_init(void);

/* 启动 PTP Master */
int ptp_start_master(void);

/* 停止 PTP */
void ptp_stop(void);

/* 获取当前时间 */
int ptp_get_time(struct timespec* ts);

/* 设置时间 */
int ptp_set_time(const struct timespec* ts);

/* 获取时钟信息 */
typedef struct {
    int64_t offset;              /* ns */
    int64_t frequency_offset;    /* ppb */
    int32_t delay;               /* ns */
} ptp_clock_info_t;

int ptp_get_clock_info(ptp_clock_info_t* info);

/* 获取统计信息 */
typedef struct {
    uint64_t sync_packets_sent;
    uint64_t announce_packets_sent;
    uint64_t delay_packets_sent;
    uint64_t sync_timeout_count;
    int32_t  last_offset;
} ptp_stats_t;

int ptp_get_stats(ptp_stats_t* stats);

#ifdef __cplusplus
}
#endif

#endif /* PTP_H */
```

---

## 3. TSN Scheduler 框架初始化

### 3.1 项目结构

```bash
cd tsn-scheduler

# 创建目录
mkdir -p src/tsn/{core,linux,utils}
mkdir -p tests/{unit,integration}
mkdir -p docs/
mkdir -p examples/

# 创建 __init__.py
touch src/__init__.py
touch src/tsn/__init__.py
touch src/tsn/core/__init__.py
touch src/tsn/linux/__init__.py
touch src/tsn/utils/__init__.py
touch tests/__init__.py
touch tests/unit/__init__.py
touch tests/integration/__init__.py
```

### 3.2 setup.py

**文件**: `tsn-scheduler/setup.py`

```python
from setuptools import setup, find_packages
import os

# 读取 README
with open("README.md", "r", encoding="utf-8") as fh:
    long_description = fh.read()

setup(
    name="tsn-scheduler",
    version="0.2.0",
    author="OpenNeuro Team",
    description="IEEE 802.1Qbv TSN Scheduler",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/OpenNeuro/OpenNeuro",
    packages=find_packages(),
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: Apache Software License",
        "Operating System :: POSIX :: Linux",
    ],
    python_requires=">=3.8",
    install_requires=[
        "numpy>=1.20",
        "pyyaml>=5.4",
    ],
    extras_require={
        "dev": [
            "pytest>=6.0",
            "pytest-cov>=2.0",
            "black>=21.0",
            "pylint>=2.0",
        ],
    },
)
```

### 3.3 初始模块

**文件**: `tsn-scheduler/src/tsn/__init__.py`

```python
"""TSN Scheduler - IEEE 802.1Qbv 实现"""

__version__ = "0.2.0"
__author__ = "OpenNeuro Team"

from .core.queue import PriorityQueue
from .core.scheduler import TSNScheduler

__all__ = ["PriorityQueue", "TSNScheduler"]
```

**文件**: `tsn-scheduler/src/tsn/core/__init__.py`

```python
"""TSN 核心模块"""

from .queue import PriorityQueue
from .scheduler import TSNScheduler
from .flow import FlowTable

__all__ = ["PriorityQueue", "TSNScheduler", "FlowTable"]
```

---

## 4. 硬件设计框架初始化

### 4.1 KiCad 项目创建

```bash
# 在 KiCad 中手动创建:
# File → New Project → zone-controller

# 或通过命令行 (如果支持):
# kicad -d zone-controller

# 结果文件:
# zone-controller/hardware/kicad/zone-controller.kicad_pro
# zone-controller/hardware/kicad/zone-controller.kicad_sch
# zone-controller/hardware/kicad/zone-controller.kicad_pcb
```

### 4.2 设计文档框架

**文件**: `zone-controller/hardware/design_notes.md`

```markdown
# Zone Controller 硬件设计文档

## 1. 设计阶段进度

### W2 目标: 原理图 50% (电源系统)

- [ ] 电源树设计
- [ ] 电源管理芯片
- [ ] 稳压电路
- [ ] 滤波电容

### W3 目标: 原理图 75% (网络部分)

- [ ] RJ45 接口
- [ ] PHY 芯片
- [ ] EMI/EMC 保护
- [ ] 晶振

### W4 目标: 原理图 100% + PCB 75%

- [ ] RK3588 和外围电路
- [ ] PCB Layout 开始
- [ ] 关键网络设计

## 2. 设计规范

### PCB 约束
- 层数: 4 层 (Power, GND, Signal, GND)
- 最小线宽: 0.25mm
- 最小间距: 0.25mm
- 差分线阻抗: 100Ω (网络)

### 器件选型
- 参考 BOM_DETAILED.md 的完整器件清单
- 优先选用国产/容易采购器件
- 价格目标: ¥361/单位

## 3. 设计检查清单

### 电源设计
- [ ] 电源树完整
- [ ] 纹波 < 100mV
- [ ] 瞬态响应 <50µs
- [ ] 反向保护
- [ ] 软启动

### 信号完整性
- [ ] 时序分析
- [ ] 串扰分析
- [ ] 反射分析
- [ ] EMI/EMC 验证

### 热管理
- [ ] 散热计算
- [ ] 温度分布
- [ ] 热焊盘设计
```

---

## 5. 测试框架初始化

### 5.1 pytest 配置

**文件**: `tests/pytest.ini`

```ini
[pytest]
testpaths = tests
python_files = test_*.py
python_classes = Test*
python_functions = test_*
addopts = 
    -v
    --tb=short
    --strict-markers
    -ra
markers =
    unit: unit tests
    integration: integration tests
    performance: performance tests
    slow: slow tests
```

### 5.2 测试工具链

**文件**: `tests/conftest.py`

```python
"""pytest 共享配置和 fixtures"""

import pytest
import sys
import os

# 添加源代码路径
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))

@pytest.fixture
def test_data_dir():
    """测试数据目录"""
    return os.path.join(os.path.dirname(__file__), 'data')

@pytest.fixture
def sample_ptp_packet():
    """样本 PTP 数据包"""
    return bytes([0] * 64)

@pytest.fixture
def sample_network_frame():
    """样本网络帧"""
    return {
        'src_mac': '00:11:22:33:44:55',
        'dst_mac': '00:AA:BB:CC:DD:EE',
        'vlan': 100,
        'priority': 3,
        'payload': b'\x00' * 64,
    }
```

---

## 6. CI/CD 流水线配置

### 6.1 GitHub Actions 工作流

**文件**: `.github/workflows/ci-ptp.yml`

```yaml
name: PTP CI

on:
  push:
    branches: [ develop, main ]
    paths:
      - 'ptp-stack/**'
      - '.github/workflows/ci-ptp.yml'
  pull_request:
    branches: [ develop ]
    paths:
      - 'ptp-stack/**'

jobs:
  build-and-test:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, windows-latest, macos-latest]
        compiler: [gcc, clang]
        exclude:
          - os: windows-latest
            compiler: clang

    steps:
    - uses: actions/checkout@v3

    - name: Install dependencies (Linux)
      if: runner.os == 'Linux'
      run: |
        sudo apt-get update
        sudo apt-get install -y build-essential cmake libgtest-dev

    - name: Install dependencies (macOS)
      if: runner.os == 'macOS'
      run: |
        brew install cmake googletest

    - name: Install dependencies (Windows)
      if: runner.os == 'Windows'
      run: |
        choco install cmake -y
        # Google Test will be fetched by CMake

    - name: Create build directory
      run: |
        cd ptp-stack
        mkdir build

    - name: Configure CMake
      run: |
        cd ptp-stack/build
        cmake ..

    - name: Build
      run: |
        cd ptp-stack/build
        cmake --build . --config Release

    - name: Run tests
      run: |
        cd ptp-stack/build
        ctest --output-on-failure

    - name: Generate coverage
      if: runner.os == 'Linux'
      run: |
        cd ptp-stack/build
        # 需要配置 gcov
        # gcovr --root .. --filter ../src

    - name: Upload coverage
      if: runner.os == 'Linux'
      uses: codecov/codecov-action@v3
```

**文件**: `.github/workflows/ci-tsn.yml`

```yaml
name: TSN CI

on:
  push:
    branches: [ develop, main ]
    paths:
      - 'tsn-scheduler/**'
      - '.github/workflows/ci-tsn.yml'
  pull_request:
    branches: [ develop ]
    paths:
      - 'tsn-scheduler/**'

jobs:
  test:
    runs-on: ubuntu-latest
    strategy:
      matrix:
        python-version: ['3.8', '3.9', '3.10', '3.11']

    steps:
    - uses: actions/checkout@v3

    - name: Set up Python
      uses: actions/setup-python@v4
      with:
        python-version: ${{ matrix.python-version }}

    - name: Install dependencies
      run: |
        python -m pip install --upgrade pip
        pip install -e ".[dev]"
        sudo apt-get install -y iproute2

    - name: Lint
      run: |
        cd tsn-scheduler
        pylint src/ --disable=all --enable=E,F --exit-zero || true

    - name: Run tests
      run: |
        cd tsn-scheduler
        pytest tests/ -v --cov=src --cov-report=xml

    - name: Upload coverage
      uses: codecov/codecov-action@v3
```

---

## 7. 初始化检查清单

### 7.1 PM 检查清单 (1月24日)

```
□ PTP Stack
  □ 目录结构创建完成
  □ CMakeLists.txt 编写完成
  □ 初始代码文件创建
  □ 编译测试通过
  □ 代码提交到 Git

□ TSN Scheduler
  □ 目录结构创建完成
  □ setup.py 编写完成
  □ 初始模块创建
  □ 导入测试通过
  □ 代码提交到 Git

□ Zone Controller
  □ 目录结构创建完成
  □ KiCad 项目创建
  □ 库配置完成
  □ 设计文档框架完成
  □ 文件提交到 Git

□ 测试框架
  □ pytest 配置完成
  □ GitHub Actions 工作流配置
  □ CI/CD 流水线测试通过
  □ 覆盖率工具配置

□ 代码审查
  □ 所有代码格式符合规范
  □ 初始代码没有错误
  □ 文档注释完整
  □ 代码可以编译/运行
```

### 7.2 各团队验证命令

**PTP Team**:
```bash
cd ptp-stack
mkdir build && cd build
cmake .. && cmake --build .
ctest
```

**TSN Team**:
```bash
cd tsn-scheduler
pip install -e .
pytest tests/
```

**硬件 Team**:
```bash
# 在 KiCad 中打开
zone-controller/hardware/kicad/zone-controller.kicad_pro
```

---

## 8. 最终确认

**1月25日下午 16:00 时间点**:

所有团队向 PM 报告:
- ✅ 开发环境就绪
- ✅ 代码框架完成
- ✅ 编译/运行成功
- ✅ CI/CD 测试通过
- ✅ 代码已提交到 Git
- ✅ 人员准备就绪

**PM 最终检查**:
```bash
cd d:\VScode专用\OpenNeuro

# 验证所有分支
git branch -a

# 验证代码框架
find . -name "CMakeLists.txt" | head -5
find . -name "setup.py" | head -5

# 验证 CI/CD
ls .github/workflows/ci-*.yml

# 输出确认报告
echo "✅ 所有框架初始化完成"
echo "📅 1/27 启动会议"
echo "🚀 1/28 编码冲刺开始"
```

---

**准备就绪，等待1月27日启动会议！** 🎯
