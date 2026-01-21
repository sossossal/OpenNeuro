cmake_minimum_required(VERSION 3.5)

project(STM32H7_Zenoh C)

# 工具链配置
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)

# STM32H7 特定设置
set(MCU "STM32H743xx")
set(CPU "-mcpu=cortex-m7")
set(FPU "-mfpu=fpv5-d16 -mfloat-abi=hard")
set(SPECS "-specs=nosys.specs")

set(CMAKE_C_FLAGS "${CPU} ${FPU} ${SPECS} -DSTM32H743xx -Wall -Wextra -O2")
set(CMAKE_EXE_LINKER_FLAGS "${CPU} ${FPU} ${SPECS}")

# 源文件
set(SOURCES
    src/main.c
    src/stm32h7_init.c
    src/eth_driver.c
    src/zenoh_client.c
    src/sensor_interface.c
    src/realtime_scheduler.c
)

# 可执行文件
add_executable(${PROJECT_NAME}.elf ${SOURCES})

# 生成 HEX 文件
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND arm-none-eabi-objcopy -O ihex 
    ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex
    COMMAND arm-none-eabi-size ${PROJECT_NAME}.elf
)

