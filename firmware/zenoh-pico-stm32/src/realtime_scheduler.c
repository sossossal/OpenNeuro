#include <stdio.h>
#include "config.h"

static const char *TAG = "Scheduler";

volatile uint32_t tick_count = 0;
volatile uint32_t sensor_task_count = 0;
volatile uint32_t zenoh_task_count = 0;

void SysTick_Handler(void) {
    tick_count++;
    
    // 触发 1kHz 传感器采样
    if (tick_count % (1000 / SENSOR_RATE) == 0) {
        sensor_task_count++;
    }
    
    // 触发 Zenoh 发送（1Hz）
    if (tick_count % 1000 == 0) {
        zenoh_task_count++;
    }
}

uint32_t scheduler_get_tick(void) {
    return tick_count;
}

uint32_t scheduler_get_sensor_count(void) {
    return sensor_task_count;
}

uint32_t scheduler_get_zenoh_count(void) {
    return zenoh_task_count;
}

