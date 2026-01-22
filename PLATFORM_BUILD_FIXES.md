# CI/CD 构建错误修复 - 第二轮

## 修复日期
2026-01-22 12:22

---

## 🔍 发现的问题

通过 GitHub Actions 日志分析，发现了 3 个具体问题：

### 1. Ubuntu: gcov 包不存在 ❌

**错误信息**:
```
E: Unable to locate package gcov
Error: Process completed with exit code 100.
```

**原因**: `gcov` 不是独立的包，它是 `gcc` 的一部分

**修复**: 从依赖列表中移除 `gcov`

**文件**: `.github/workflows/ci-ptp.yml`

---

### 2. macOS: 缺少头文件 ❌

**错误信息**:
```
unknown type name 'size_t' in ptp-stack/src/slave/slave.h
call to undeclared function 'NTOHS' in ptp-stack/src/protocol/delay.c
call to undeclared function 'HTONS'
call to undeclared function 'HTONL'
```

**原因**: 
- 缺少 `<stddef.h>` (定义 `size_t`)
- 缺少 `<arpa/inet.h>` (定义网络字节序函数)
- 缺少 `<string.h>` (定义 `memset`, `memcpy`)

**修复**:

1. **slave.h**: 添加 `#include <stddef.h>`

2. **delay.c**: 添加网络头文件和宏定义
```c
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

// Define macros if not already defined
#ifndef NTOHS
#define NTOHS(x) ntohs(x)
#endif
#ifndef HTONS
#define HTONS(x) htons(x)
#endif
#ifndef HTONL
#define HTONL(x) htonl(x)
#endif
```

**文件**: 
- `ptp-stack/src/slave/slave.h`
- `ptp-stack/src/protocol/delay.c`

---

### 3. Windows: PowerShell 多行命令语法错误 ❌

**错误信息**:
```
The term '-DCMAKE_BUILD_TYPE=Release' is not recognized as a name of a cmdlet, 
function, script file, or executable program.
```

**原因**: PowerShell 不支持 Unix 风格的反斜杠续行符 `\`

**修复**: 将多行命令改为单行

**之前**:
```yaml
run: |
  cmake .. \
    -DCMAKE_BUILD_TYPE=${{ env.BUILD_TYPE }} \
    -DCMAKE_CXX_FLAGS_RELEASE="-O3 -DNDEBUG" \
    -DCMAKE_C_FLAGS_RELEASE="-O3 -DNDEBUG"
```

**之后**:
```yaml
run: cmake .. -DCMAKE_BUILD_TYPE=${{ env.BUILD_TYPE }} -DCMAKE_CXX_FLAGS_RELEASE="-O3 -DNDEBUG" -DCMAKE_C_FLAGS_RELEASE="-O3 -DNDEBUG"
```

**文件**: `.github/workflows/ci-ptp.yml`

---

## 📝 修改的文件

### 1. `.github/workflows/ci-ptp.yml`
- 移除 `gcov` 包
- 修复 Windows CMake 命令语法

### 2. `ptp-stack/src/slave/slave.h`
- 添加 `#include <stddef.h>`

### 3. `ptp-stack/src/protocol/delay.c`
- 添加 `#include <string.h>`
- 添加 `#include <arpa/inet.h>` (Unix)
- 添加 `#include <winsock2.h>` (Windows)
- 定义 NTOHS, HTONS, HTONL 宏

---

## ✅ 预期结果

修复后，所有平台应该能够成功构建：

- ✅ Ubuntu (gcc) - 依赖安装成功
- ✅ Ubuntu (clang) - 依赖安装成功
- ✅ macOS (clang) - 编译成功（头文件完整）
- ✅ Windows (gcc) - CMake 配置成功
- ✅ Static Analysis - 构建成功

---

## 🎯 提交命令

```powershell
cd "D:\新建文件夹\OpenNeuro"

git add .github/workflows/ci-ptp.yml
git add ptp-stack/src/slave/slave.h
git add ptp-stack/src/protocol/delay.c

git commit -m "fix: resolve platform-specific build errors

- Remove non-existent gcov package from Ubuntu deps
- Add stddef.h to slave.h for size_t definition
- Add network headers for byte order functions
- Fix Windows PowerShell multi-line command syntax

Fixes:
- Ubuntu: E: Unable to locate package gcov
- macOS: unknown type name 'size_t'
- macOS: undeclared functions NTOHS/HTONS/HTONL
- Windows: PowerShell command syntax error

All platforms should now build successfully."

git push origin main
```

---

## 📊 修复进度

```
第一轮修复: Actions v3 → v4 ✅
第二轮修复: CodeQL v2 → v3 ✅
第三轮修复: 启用测试系统 ✅
第四轮修复: 平台特定错误 ⏳ (当前)

预计完成度: 100%
```

---

**修复人**: OpenNeuro Team  
**修复日期**: 2026-01-22 12:22  
**状态**: 待提交
