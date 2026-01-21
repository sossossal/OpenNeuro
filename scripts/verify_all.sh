#!/bin/bash
# OpenNeuro 自动化验证脚本
# 用于快速验证所有组件是否正常工作

set -e  # 遇到错误立即退出

echo "========================================="
echo "OpenNeuro 自动化验证脚本"
echo "版本: v0.2.0-alpha.1"
echo "========================================="
echo ""

# 颜色定义
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# 计数器
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# 测试函数
run_test() {
    local test_name=$1
    local test_command=$2
    
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    echo -n "[$TOTAL_TESTS] $test_name ... "
    
    if eval "$test_command" > /dev/null 2>&1; then
        echo -e "${GREEN}PASS${NC}"
        PASSED_TESTS=$((PASSED_TESTS + 1))
        return 0
    else
        echo -e "${RED}FAIL${NC}"
        FAILED_TESTS=$((FAILED_TESTS + 1))
        return 1
    fi
}

# 1. 检查构建目录
echo "=== 阶段 1: 检查构建环境 ==="
run_test "PTP Stack 构建目录存在" "test -d ptp-stack/build"
run_test "TSN Scheduler 构建目录存在" "test -d tsn-scheduler/build"
run_test "Zenoh Bridge 构建目录存在" "test -d zenoh-bridge/build"
echo ""

# 2. 检查可执行文件
echo "=== 阶段 2: 检查可执行文件 ==="
if [ "$(uname)" == "Linux" ]; then
    run_test "PTP 基准测试程序存在" "test -f ptp-stack/build/bench_ptp"
    run_test "Zenoh HAL 测试程序存在" "test -f zenoh-bridge/build/test_zenoh_hal"
    run_test "QoS 映射测试程序存在" "test -f zenoh-bridge/build/test_qos_mapping"
else
    echo -e "${YELLOW}[SKIP] Windows 平台，跳过可执行文件检查${NC}"
fi
echo ""

# 3. 运行 PTP 基准测试
echo "=== 阶段 3: PTP 性能测试 ==="
if [ "$(uname)" == "Linux" ]; then
    if ./ptp-stack/build/bench_ptp | grep -q "PASS"; then
        echo -e "[$((TOTAL_TESTS + 1))] PTP 基准测试 ... ${GREEN}PASS${NC}"
        TOTAL_TESTS=$((TOTAL_TESTS + 1))
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        echo -e "[$((TOTAL_TESTS + 1))] PTP 基准测试 ... ${RED}FAIL${NC}"
        TOTAL_TESTS=$((TOTAL_TESTS + 1))
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi
else
    echo -e "${YELLOW}[SKIP] Windows 平台，跳过 PTP 测试${NC}"
fi
echo ""

# 4. 运行 TSN Python 测试
echo "=== 阶段 4: TSN Python 绑定测试 ==="
cd tsn-scheduler
if python3 test_bindings.py 2>&1 | grep -q "All tests passed"; then
    echo -e "[$((TOTAL_TESTS + 1))] TSN Python 绑定 ... ${GREEN}PASS${NC}"
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    PASSED_TESTS=$((PASSED_TESTS + 1))
else
    echo -e "[$((TOTAL_TESTS + 1))] TSN Python 绑定 ... ${RED}FAIL${NC}"
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    FAILED_TESTS=$((FAILED_TESTS + 1))
fi
cd ..
echo ""

# 5. 运行 Zenoh 测试
echo "=== 阶段 5: Zenoh Bridge 测试 ==="
if [ "$(uname)" == "Linux" ]; then
    if ./zenoh-bridge/build/test_zenoh_hal | grep -q "All Tests Passed"; then
        echo -e "[$((TOTAL_TESTS + 1))] Zenoh HAL 测试 ... ${GREEN}PASS${NC}"
        TOTAL_TESTS=$((TOTAL_TESTS + 1))
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        echo -e "[$((TOTAL_TESTS + 1))] Zenoh HAL 测试 ... ${RED}FAIL${NC}"
        TOTAL_TESTS=$((TOTAL_TESTS + 1))
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi
    
    if ./zenoh-bridge/build/test_qos_mapping | grep -q "All QoS Mapping Tests Passed"; then
        echo -e "[$((TOTAL_TESTS + 1))] QoS 映射测试 ... ${GREEN}PASS${NC}"
        TOTAL_TESTS=$((TOTAL_TESTS + 1))
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        echo -e "[$((TOTAL_TESTS + 1))] QoS 映射测试 ... ${RED}FAIL${NC}"
        TOTAL_TESTS=$((TOTAL_TESTS + 1))
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi
else
    echo -e "${YELLOW}[SKIP] Windows 平台，跳过 Zenoh 测试${NC}"
fi
echo ""

# 6. 检查文档
echo "=== 阶段 6: 文档完整性检查 ==="
run_test "README.md 存在" "test -f README.md"
run_test "API_REFERENCE.md 存在" "test -f API_REFERENCE.md"
run_test "PERFORMANCE_REPORT.md 存在" "test -f PERFORMANCE_REPORT.md"
run_test "VERIFICATION_GUIDE.md 存在" "test -f VERIFICATION_GUIDE.md"
echo ""

# 7. 总结
echo "========================================="
echo "验证总结"
echo "========================================="
echo "总测试数: $TOTAL_TESTS"
echo -e "通过: ${GREEN}$PASSED_TESTS${NC}"
echo -e "失败: ${RED}$FAILED_TESTS${NC}"
echo ""

if [ $FAILED_TESTS -eq 0 ]; then
    echo -e "${GREEN}✅ 所有测试通过！${NC}"
    echo "OpenNeuro 已准备好使用。"
    exit 0
else
    echo -e "${RED}❌ 部分测试失败${NC}"
    echo "请检查失败的测试并重新编译。"
    exit 1
fi
