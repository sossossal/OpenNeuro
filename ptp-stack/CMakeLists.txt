cmake_minimum_required(VERSION 3.20)
project(ptp-stack C CXX)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()


set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 编译选项
if(MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -fPIC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -fPIC")
endif()

# 源文件
file(GLOB COMMON_SOURCES "src/common/*.c")
file(GLOB MASTER_SOURCES "src/master/*.c")
file(GLOB SLAVE_SOURCES "src/slave/*.c")
file(GLOB PROTOCOL_SOURCES "src/protocol/*.c")
set(PLATFORM_SOURCES "src/platform/platform.c")

set(PTP_SOURCES ${COMMON_SOURCES} ${MASTER_SOURCES} ${SLAVE_SOURCES} ${PROTOCOL_SOURCES} ${PLATFORM_SOURCES})

# 库
add_library(ptp_core STATIC ${PTP_SOURCES})
target_include_directories(ptp_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# 测试
# Test Section Disabled for Benchmark Focus
# if(NOT GTest_FOUND)
#     message(STATUS "GTest not found, will fetch from repository")
#     include(FetchContent)
#     FetchContent_Declare(googletest
#         URL https://github.com/google/googletest/archive/release-1.12.0.zip
#     )
#     FetchContent_MakeAvailable(googletest)
# endif()
# 
# file(GLOB TEST_SOURCES "tests/unit/test_*.cpp")
# foreach(TEST_SOURCE ${TEST_SOURCES})
#     get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
#     add_executable(${TEST_NAME} ${TEST_SOURCE})
#     target_link_libraries(${TEST_NAME} ptp_core gtest gtest_main)
#     add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
# endforeach()

# 覆盖率
if(CMAKE_BUILD_TYPE MATCHES Debug AND NOT MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
endif()

# Benchmarks
add_executable(bench_ptp tests/benchmark/bench_ptp.c)
target_link_libraries(bench_ptp ptp_core)

# Integration Tests
add_executable(test_master_slave tests/integration/test_master_slave.c)
target_link_libraries(test_master_slave ptp_core)

