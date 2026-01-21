# Zenoh 真实库集成指南 (更新版)

## 概述

本文档说明如何将 OpenNeuro Zenoh Bridge 从 Mock 实现切换到真实 Zenoh C 库。

---

## 方法 1: 使用预编译的 Zenoh C 库 (推荐)

### 步骤 1: 下载 Zenoh C

```bash
# Linux
wget https://github.com/eclipse-zenoh/zenoh-c/releases/download/0.10.1-rc/zenoh-c-0.10.1-rc-x86_64-unknown-linux-gnu.zip
unzip zenoh-c-0.10.1-rc-x86_64-unknown-linux-gnu.zip
sudo cp -r include/* /usr/local/include/
sudo cp -r lib/* /usr/local/lib/
sudo ldconfig

# macOS
brew install zenoh-c

# Windows
# 下载并解压到 C:\zenoh-c\
```

### 步骤 2: 更新 CMakeLists.txt

```cmake
# zenoh-bridge/CMakeLists.txt

option(USE_ZENOH_PICO "Use Zenoh-Pico instead of full Zenoh" OFF)
option(USE_REAL_ZENOH "Use real Zenoh library (not mock)" ON)  # 新增

if(USE_ZENOH_PICO)
    message(STATUS "Using Zenoh-Pico HAL")
    set(ZENOH_HAL_SRC src/zenoh_hal_pico.c)
    
    if(USE_REAL_ZENOH)
        find_package(zenoh-pico REQUIRED)
        set(ZENOH_LIB zenoh-pico::lib)
        add_definitions(-DUSE_REAL_ZENOH)
    else()
        set(ZENOH_LIB "")
    endif()
else()
    message(STATUS "Using Full Zenoh HAL")
    set(ZENOH_HAL_SRC src/zenoh_hal_full.c)
    
    if(USE_REAL_ZENOH)
        # 查找 Zenoh C 库
        find_library(ZENOHC_LIB zenohc PATHS /usr/local/lib C:/zenoh-c/lib)
        find_path(ZENOHC_INCLUDE zenoh.h PATHS /usr/local/include C:/zenoh-c/include)
        
        if(ZENOHC_LIB AND ZENOHC_INCLUDE)
            message(STATUS "Found Zenoh C: ${ZENOHC_LIB}")
            set(ZENOH_LIB ${ZENOHC_LIB})
            include_directories(${ZENOHC_INCLUDE})
            add_definitions(-DUSE_REAL_ZENOH)
        else()
            message(WARNING "Zenoh C not found, using mock")
            set(ZENOH_LIB "")
        endif()
    else()
        set(ZENOH_LIB "")
    endif()
endif()
```

### 步骤 3: 重新编译

```bash
cd zenoh-bridge
rm -rf build
cmake -S . -B build -DUSE_REAL_ZENOH=ON
cmake --build build
```

---

## 方法 2: 从源码编译 Zenoh C

### 步骤 1: 安装 Rust

```bash
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
source $HOME/.cargo/env
```

### 步骤 2: 编译 Zenoh C

```bash
git clone https://github.com/eclipse-zenoh/zenoh-c.git
cd zenoh-c
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --target install
```

### 步骤 3: 验证安装

```bash
pkg-config --modversion zenohc
# 应输出版本号，例如 0.10.1
```

---

## 测试真实 Zenoh

### 测试 1: 启动 Zenoh Router

```bash
# 下载 Zenoh Router
wget https://github.com/eclipse-zenoh/zenoh/releases/download/0.10.1-rc/zenohd-0.10.1-rc-x86_64-unknown-linux-gnu.zip
unzip zenohd-0.10.1-rc-x86_64-unknown-linux-gnu.zip
chmod +x zenohd

# 启动 Router
./zenohd
```

### 测试 2: 运行 HAL 测试

```bash
cd zenoh-bridge/build
./test_zenoh_hal
```

**预期输出** (真实 Zenoh):
```
=== Zenoh HAL Test ===
[Zenoh HAL Real] Session opened: tcp/127.0.0.1:7447
[PASS] Session opened
[Zenoh HAL Real] Publisher declared: test/topic
[PASS] Publisher declared
[Zenoh HAL Real] Put 4 bytes to 'test/topic'
[PASS] Data published
...
```

### 测试 3: Pub/Sub 验证

**Terminal 1** (Publisher):
```bash
./build/test_zenoh_hal
```

**Terminal 2** (Subscriber):
```bash
# 使用 Zenoh 命令行工具
zenoh-cli sub 'test/topic'
```

应该能看到发布的数据。

---

## 故障排查

### 问题 1: 找不到 zenoh.h

**解决方案**:
```bash
# 检查头文件位置
find /usr -name zenoh.h

# 手动指定路径
cmake -S . -B build -DUSE_REAL_ZENOH=ON \
  -DCMAKE_INCLUDE_PATH=/path/to/zenoh/include
```

### 问题 2: 链接错误

**解决方案**:
```bash
# 检查库文件
find /usr -name libzenohc.so

# 更新 LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
sudo ldconfig
```

### 问题 3: 运行时找不到库

**解决方案**:
```bash
# Linux
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH

# macOS
export DYLD_LIBRARY_PATH=/usr/local/lib:$DYLD_LIBRARY_PATH

# 或永久添加
echo 'export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH' >> ~/.bashrc
```

---

## 性能对比

| 实现 | 延迟 | 吞吐量 | 内存 |
|------|------|--------|------|
| Mock | N/A | N/A | ~1KB |
| Real Zenoh | <100µs | >10K msg/s | ~10MB |

---

## 下一步

1. 集成真实 Zenoh 后，运行完整的 Pub/Sub 测试
2. 测试 QoS 映射在真实网络中的表现
3. 部署多节点 Zenoh 网络
4. 集成到 AGI-Walker 项目

---

**更新日期**: 2026-01-21  
**Zenoh 版本**: 0.10.1-rc
