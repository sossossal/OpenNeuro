@echo off
REM OpenNeuro Windows 自动化验证脚本
REM 用于快速验证所有组件是否正常工作

echo =========================================
echo OpenNeuro 自动化验证脚本 (Windows)
echo 版本: v0.2.0-alpha.1
echo =========================================
echo.

set TOTAL_TESTS=0
set PASSED_TESTS=0
set FAILED_TESTS=0

REM 1. 检查构建目录
echo === 阶段 1: 检查构建环境 ===
call :run_test "PTP Stack 构建目录存在" "if exist ptp-stack\build (exit /b 0) else (exit /b 1)"
call :run_test "TSN Scheduler 构建目录存在" "if exist tsn-scheduler\build (exit /b 0) else (exit /b 1)"
call :run_test "Zenoh Bridge 构建目录存在" "if exist zenoh-bridge\build (exit /b 0) else (exit /b 1)"
echo.

REM 2. 检查可执行文件
echo === 阶段 2: 检查可执行文件 ===
call :run_test "PTP 基准测试程序存在" "if exist ptp-stack\build\Release\bench_ptp.exe (exit /b 0) else (exit /b 1)"
call :run_test "Zenoh HAL 测试程序存在" "if exist zenoh-bridge\build\Release\test_zenoh_hal.exe (exit /b 0) else (exit /b 1)"
call :run_test "QoS 映射测试程序存在" "if exist zenoh-bridge\build\Release\test_qos_mapping.exe (exit /b 0) else (exit /b 1)"
echo.

REM 3. 运行 PTP 基准测试
echo === 阶段 3: PTP 性能测试 ===
if exist ptp-stack\build\Release\bench_ptp.exe (
    ptp-stack\build\Release\bench_ptp.exe > nul 2>&1
    if %ERRORLEVEL% EQU 0 (
        echo [PASS] PTP 基准测试
        set /a PASSED_TESTS+=1
    ) else (
        echo [FAIL] PTP 基准测试
        set /a FAILED_TESTS+=1
    )
    set /a TOTAL_TESTS+=1
) else (
    echo [SKIP] PTP 基准测试程序不存在
)
echo.

REM 4. 运行 TSN Python 测试
echo === 阶段 4: TSN Python 绑定测试 ===
cd tsn-scheduler
python test_bindings.py > nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo [PASS] TSN Python 绑定
    set /a PASSED_TESTS+=1
) else (
    echo [FAIL] TSN Python 绑定
    set /a FAILED_TESTS+=1
)
set /a TOTAL_TESTS+=1
cd ..
echo.

REM 5. 运行 Zenoh 测试
echo === 阶段 5: Zenoh Bridge 测试 ===
if exist zenoh-bridge\build\Release\test_zenoh_hal.exe (
    zenoh-bridge\build\Release\test_zenoh_hal.exe > nul 2>&1
    if %ERRORLEVEL% EQU 0 (
        echo [PASS] Zenoh HAL 测试
        set /a PASSED_TESTS+=1
    ) else (
        echo [FAIL] Zenoh HAL 测试
        set /a FAILED_TESTS+=1
    )
    set /a TOTAL_TESTS+=1
)

if exist zenoh-bridge\build\Release\test_qos_mapping.exe (
    zenoh-bridge\build\Release\test_qos_mapping.exe > nul 2>&1
    if %ERRORLEVEL% EQU 0 (
        echo [PASS] QoS 映射测试
        set /a PASSED_TESTS+=1
    ) else (
        echo [FAIL] QoS 映射测试
        set /a FAILED_TESTS+=1
    )
    set /a TOTAL_TESTS+=1
)
echo.

REM 6. 检查文档
echo === 阶段 6: 文档完整性检查 ===
call :run_test "README.md 存在" "if exist README.md (exit /b 0) else (exit /b 1)"
call :run_test "API_REFERENCE.md 存在" "if exist API_REFERENCE.md (exit /b 0) else (exit /b 1)"
call :run_test "PERFORMANCE_REPORT.md 存在" "if exist PERFORMANCE_REPORT.md (exit /b 0) else (exit /b 1)"
call :run_test "VERIFICATION_GUIDE.md 存在" "if exist VERIFICATION_GUIDE.md (exit /b 0) else (exit /b 1)"
echo.

REM 7. 总结
echo =========================================
echo 验证总结
echo =========================================
echo 总测试数: %TOTAL_TESTS%
echo 通过: %PASSED_TESTS%
echo 失败: %FAILED_TESTS%
echo.

if %FAILED_TESTS% EQU 0 (
    echo ✅ 所有测试通过！
    echo OpenNeuro 已准备好使用。
    exit /b 0
) else (
    echo ❌ 部分测试失败
    echo 请检查失败的测试并重新编译。
    exit /b 1
)

REM 测试函数
:run_test
set /a TOTAL_TESTS+=1
echo [%TOTAL_TESTS%] %~1 ...
%~2
if %ERRORLEVEL% EQU 0 (
    echo [PASS]
    set /a PASSED_TESTS+=1
) else (
    echo [FAIL]
    set /a FAILED_TESTS+=1
)
exit /b 0
