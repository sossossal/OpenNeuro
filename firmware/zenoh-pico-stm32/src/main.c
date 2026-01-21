#include <stdio.h>
#include "stm32h7xx.h"
#include "config.h"
#include "stm32h7_init.h"
#include "eth_driver.h"
#include "zenoh_client.h"
#include "sensor_interface.h"
#include "realtime_scheduler.h"

static const char *TAG = "Main";

void main(void) {
    // 硬件初始化
    stm32h7_init();
    printf("\n");
    printf("=== OpenNeuro STM32H7 Zenoh-Pico Demo v0.1.0 ===\n");
    printf("Device: %s\n", DEVICE_NAME);
    printf("Core Clock: %lu Hz\n\n", SystemCoreClock);

    // 初始化各模块
    eth_init();
    printf("\n");
    
    sensor_init();
    printf("\n");
    
    zenoh_init();
    printf("\n");

    // 模拟主循环
    printf("[%s] Entering main loop\n\n", TAG);
    
    uint32_t last_zenoh_publish = 0;
    uint32_t last_sensor_read = 0;

    while (1) {
        uint32_t tick = scheduler_get_tick();
        
        // 每 10ms 读取一次传感器
        if (tick - last_sensor_read >= 10) {
            last_sensor_read = tick;
            
            sensor_data_t sensor_data;
            sensor_read_all(&sensor_data);
            
            // 每 100ms 打印一次
            static uint32_t print_count = 0;
            if (print_count++ % 10 == 0) {
                printf("[SENSOR] Current: %.2f mA, Voltage: %.2f mV, Time: %u µs\n",
                       sensor_data.current_ma, sensor_data.voltage_mv, sensor_data.timestamp_us);
            }
        }
        
        // 每 1000ms 发送一次 Zenoh 消息
        if (tick - last_zenoh_publish >= 1000) {
            last_zenoh_publish = tick;
            
            char payload[128];
            snprintf(payload, sizeof(payload),
                    "{\"device\": \"%s\", \"uptime\": %u, \"status\": \"ok\"}",
                    DEVICE_NAME, tick);
            
            zenoh_publish("rt/device/stm32h7_001/status", (const uint8_t*)payload, strlen(payload));
            
            // 闪烁 LED 表示活动
            led_toggle();
        }
    }

    return 0;
}

