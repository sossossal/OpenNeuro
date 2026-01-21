cmake_minimum_required(VERSION 3.20)
project(tsn-scheduler C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

if(MSVC)
    add_compile_options(/W4)
    add_definitions(-D_PLATFORM_MOCK) # Assume Windows/Mock environment
else()
    add_compile_options(-Wall -Wextra -pedantic -fPIC)
    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        add_definitions(-D__linux__)
    endif()
endif()

# Include directories
include_directories(src)

# Source files
file(GLOB CORE_SOURCES "src/core/*.c")
file(GLOB LINUX_SOURCES "src/linux/*.c")

# Library
add_library(tsn_scheduler STATIC ${CORE_SOURCES} ${LINUX_SOURCES})

# Benchmarks
add_executable(bench_scheduler tests/benchmark/bench_scheduler.c)
target_link_libraries(bench_scheduler tsn_scheduler)

# Tests (Disabled for now to avoid GTest complexity in mixed env)
# enable_testing()
# ... GTest logic ...
