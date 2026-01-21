cmake_minimum_required(VERSION 3.15)
project(zenoh-bridge C)

# 选项
option(USE_ZENOH_PICO "Use Zenoh-Pico instead of full Zenoh" OFF)
option(USE_REAL_ZENOH "Use real Zenoh library (not mock)" OFF)

# 设置 C 标准
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 源文件
set(ZENOH_BRIDGE_SOURCES
    src/ptp_publisher.c
    src/tsn_qos_bridge.c
)

# 根据选项选择 HAL 实现
if(USE_ZENOH_PICO)
    message(STATUS "Using Zenoh-Pico HAL")
    list(APPEND ZENOH_BRIDGE_SOURCES src/zenoh_hal_pico.c)
    
    if(USE_REAL_ZENOH)
        # 查找 Zenoh-Pico 库
        find_library(ZENOHPICO_LIB zenohpico)
        find_path(ZENOHPICO_INCLUDE zenoh-pico.h)
        
        if(ZENOHPICO_LIB AND ZENOHPICO_INCLUDE)
            message(STATUS "Found Zenoh-Pico: ${ZENOHPICO_LIB}")
            set(ZENOH_LIB ${ZENOHPICO_LIB})
            include_directories(${ZENOHPICO_INCLUDE})
            add_definitions(-DUSE_REAL_ZENOH)
        else()
            message(WARNING "Zenoh-Pico not found, using mock")
            set(ZENOH_LIB "")
        endif()
    else()
        message(STATUS "Using mock Zenoh-Pico")
        set(ZENOH_LIB "")
    endif()
else()
    message(STATUS "Using Full Zenoh HAL")
    list(APPEND ZENOH_BRIDGE_SOURCES src/zenoh_hal_full.c)
    
    if(USE_REAL_ZENOH)
        # 查找 Zenoh C 库
        find_library(ZENOHC_LIB zenohc 
            PATHS /usr/local/lib C:/zenoh-c/lib
            DOC "Zenoh C library")
        find_path(ZENOHC_INCLUDE zenoh.h 
            PATHS /usr/local/include C:/zenoh-c/include
            DOC "Zenoh C include directory")
        
        if(ZENOHC_LIB AND ZENOHC_INCLUDE)
            message(STATUS "Found Zenoh C: ${ZENOHC_LIB}")
            set(ZENOH_LIB ${ZENOHC_LIB})
            include_directories(${ZENOHC_INCLUDE})
            add_definitions(-DUSE_REAL_ZENOH)
        else()
            message(WARNING "Zenoh C not found, using mock")
            message(STATUS "  Searched in: /usr/local/lib, C:/zenoh-c/lib")
            message(STATUS "  To use real Zenoh, install from:")
            message(STATUS "    https://github.com/eclipse-zenoh/zenoh-c/releases")
            set(ZENOH_LIB "")
        endif()
    else()
        message(STATUS "Using mock Zenoh")
        set(ZENOH_LIB "")
    endif()
endif()

# 创建库
add_library(zenoh_bridge STATIC ${ZENOH_BRIDGE_SOURCES})

# 链接 Zenoh 库 (如果找到)
if(ZENOH_LIB)
    target_link_libraries(zenoh_bridge ${ZENOH_LIB})
endif()

# 包含目录
target_include_directories(zenoh_bridge PUBLIC src)

# 测试程序
add_executable(test_zenoh_hal tests/test_hal.c)
target_link_libraries(test_zenoh_hal zenoh_bridge)

add_executable(test_qos_mapping tests/test_qos_mapping.c)
target_link_libraries(test_qos_mapping zenoh_bridge)

add_executable(test_pubsub tests/test_pubsub.c)
target_link_libraries(test_pubsub zenoh_bridge)
if(UNIX)
    target_link_libraries(test_pubsub pthread)
endif()


# 安装
install(TARGETS zenoh_bridge
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib)

install(FILES src/zenoh_hal.h src/tsn_qos_bridge.h
    DESTINATION include/zenoh-bridge)

# 打印配置摘要
message(STATUS "")
message(STATUS "=== Zenoh Bridge Configuration ===")
message(STATUS "  Zenoh Mode: ${USE_ZENOH_PICO}")
message(STATUS "  Real Zenoh: ${USE_REAL_ZENOH}")
if(ZENOH_LIB)
    message(STATUS "  Zenoh Library: ${ZENOH_LIB}")
else()
    message(STATUS "  Zenoh Library: Mock (no real library)")
endif()
message(STATUS "===================================")
message(STATUS "")
