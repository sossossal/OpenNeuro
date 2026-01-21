#include <stdio.h>
#include <stdint.h>
#include "config.h"

static const char *TAG = "Sensor";

volatile uint32_t adc_buffer[16];
volatile uint16_t adc_read_index = 0;

void sensor_init(void) {
    printf("[%s] ADC initialized on channel %d\n", TAG, SENSOR_CHANNEL);
    printf("[%s] Sampling rate: %d Hz\n", TAG, SENSOR_RATE);
}

uint16_t sensor_read_adc(void) {
    // 模拟 ADC 读取
    static uint16_t adc_value = 0;
    adc_value = (adc_value + 123) % 4096;  // 伪随机值
    return adc_value;
}

float sensor_read_voltage(void) {
    uint16_t raw = sensor_read_adc();
    // 3.3V 对应 4095
    return (raw * 3300.0) / 4095.0;
}

void sensor_read_all(sensor_data_t *data) {
    if (!data) return;
    
    // 读取 ADC
    uint16_t raw_adc = sensor_read_adc();
    
    // 转换为物理量（示例：电流 - 0-4095 对应 0-10A）
    data->current_ma = (raw_adc / 4095.0) * 10000.0;
    
    // 电压（示例）
    data->voltage_mv = sensor_read_voltage();
    
    // 时间戳（µs）
    static uint32_t timestamp = 0;
    data->timestamp_us = timestamp++;
}

