# OpenNeuro CI/CD 配置说明

## 概述

本文档说明 OpenNeuro 项目的 CI/CD 管道配置和使用方法。

## 工作流文件

### 主 CI/CD 管道
**文件**: `.github/workflows/ci-cd.yml`

**触发条件**:
- Push 到 `main` 或 `develop` 分支
- Pull Request 到 `main` 分支
- 创建 Release (Tag)

## 构建矩阵

### 支持的平台
- **Ubuntu Latest** (Linux)
- **Windows Latest**

### 构建类型
- **Release** (优化构建)
- **Debug** (调试符号)

### Zenoh 模式
- **Full Zenoh** (`USE_ZENOH_PICO=OFF`)
- **Zenoh-Pico** (`USE_ZENOH_PICO=ON`)

## Job 说明

### 1. ptp-stack
**目的**: 构建和测试 PTP 协议栈

**步骤**:
1. 安装依赖 (CMake, GCC/MSVC, Ninja)
2. 配置 CMake
3. 构建 PTP Stack
4. 运行 PTP Benchmark (`bench_ptp`)
5. 上传构建产物

**产物**: `ptp-stack-{os}-{build_type}`

---

### 2. tsn-scheduler
**目的**: 构建和测试 TSN 调度器

**步骤**:
1. 安装依赖 (CMake, Python)
2. 构建 TSN Scheduler
3. 构建 Python 绑定
4. 测试 Python 绑定 (`test_bindings.py`)
5. 上传构建产物

**产物**: `tsn-scheduler-{os}`

---

### 3. zenoh-bridge
**目的**: 构建和测试 Zenoh Bridge (两种模式)

**步骤**:
1. 配置 CMake (Full/Pico 模式)
2. 构建 Zenoh Bridge
3. 运行 HAL 测试 (`test_zenoh_hal`)
4. 运行 QoS 映射测试 (`test_qos_mapping`)

**产物**: `zenoh-bridge-{os}-{mode}`

---

### 4. integration
**目的**: 运行集成测试

**依赖**: ptp-stack, tsn-scheduler, zenoh-bridge

**步骤**:
1. 下载所有构建产物
2. 运行集成测试 (待实现)

---

### 5. docs
**目的**: 验证文档完整性

**检查文件**:
- `API_REFERENCE.md`
- `PERFORMANCE_REPORT.md`
- `RELEASE_NOTES_v0.2.0-alpha.1.md`
- `README.md`

---

### 6. release
**目的**: 创建发布包 (仅在 Tag 时触发)

**触发条件**: `refs/tags/v*`

**步骤**:
1. 打包所有组件
2. 包含文档
3. 创建 `.tar.gz` 压缩包
4. 上传为 Release Asset

---

## 本地测试

### 运行完整构建

```bash
# PTP Stack
cd ptp-stack
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/bench_ptp

# TSN Scheduler
cd tsn-scheduler
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build
python setup.py build_ext --inplace
python test_bindings.py

# Zenoh Bridge (Full)
cd zenoh-bridge
cmake -B build -S . -DUSE_ZENOH_PICO=OFF
cmake --build build
./build/test_zenoh_hal
./build/test_qos_mapping

# Zenoh Bridge (Pico)
cmake -B build-pico -S . -DUSE_ZENOH_PICO=ON
cmake --build build-pico
```

---

## 发布流程

### 创建 Release

1. **更新版本号**
   ```bash
   # 确保所有文档中的版本号一致
   grep -r "v0.2.0-alpha.1" .
   ```

2. **创建 Git Tag**
   ```bash
   git tag -a v0.2.0-alpha.1 -m "Release v0.2.0-alpha.1"
   git push origin v0.2.0-alpha.1
   ```

3. **CI/CD 自动触发**
   - GitHub Actions 自动运行所有测试
   - 构建发布包
   - 上传到 GitHub Releases

4. **手动发布 (可选)**
   ```bash
   # 创建发布包
   ./scripts/create_release.sh v0.2.0-alpha.1
   ```

---

## 故障排查

### 常见问题

**Q: Windows 构建失败 - `usleep` 未定义**
A: 使用 `Sleep()` 替代 (已在代码中处理)

**Q: Python 绑定构建失败**
A: 确保安装了 `python3-dev` (Linux) 或 Python 开发工具 (Windows)

**Q: Zenoh 链接错误**
A: 当前使用 Mock 实现，不需要真实 Zenoh 库

---

## 性能基准

CI/CD 管道会自动运行性能基准测试：

| 测试 | 通过条件 |
|------|----------|
| PTP Benchmark | 延迟 < 20µs |
| TSN Scheduler | 编译通过 |
| Zenoh HAL | 所有测试通过 |
| QoS Mapping | 所有映射正确 |

---

## 下一步改进

### M2.2 计划
- [ ] 添加真实 Zenoh 库集成测试
- [ ] 添加硬件时间戳测试 (需要硬件)
- [ ] 添加多节点网络测试
- [ ] 添加代码覆盖率报告
- [ ] 添加静态分析 (cppcheck, clang-tidy)

---

## 联系方式

**问题反馈**: https://github.com/your-org/OpenNeuro/issues  
**CI/CD 维护**: OpenNeuro DevOps Team
