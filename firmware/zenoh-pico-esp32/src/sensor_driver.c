#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"

#include "config.h"
#include "zenoh_client.h"

static const char *TAG = "Sensor-Driver";

static adc_oneshot_unit_handle_t adc1_handle;

void sensor_init(void) {
    // ADC1 初始化
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };
    adc_oneshot_new_unit(&init_config1, &adc1_handle);

    // 配置 ADC 通道（GPIO 36 = ADC1_CH0）
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_0,
    };
    adc_oneshot_config_channel(adc1_handle, SENSOR_ADC_PIN, &config);

    ESP_LOGI(TAG, "ADC initialized on channel %d", SENSOR_ADC_PIN);
}

void sensor_task(void *arg) {
    ESP_LOGI(TAG, "Sensor acquisition task started at %d Hz", SENSOR_RATE);

    uint32_t sample_count = 0;
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(SENSOR_READ_INTERVAL_MS));

        // 读取 ADC 值
        int adc_value = 0;
        adc_oneshot_read(adc1_handle, SENSOR_ADC_PIN, &adc_value);

        // 转换为物理量（示例：温度）
        // 假设 ADC 值 0-4095 对应 0-50°C
        float temperature = (adc_value / 4095.0) * 50.0;

        // 构建消息
        char payload[128];
        snprintf(payload, sizeof(payload),
                "{\"temperature\": %.2f, \"adc\": %d, \"sample\": %u}",
                temperature, adc_value, sample_count);

        // 发布传感器数据
        zenoh_publish_sensor("rt/device/esp32_001/sensor/temperature", payload);

        sample_count++;
        if (sample_count % SENSOR_RATE == 0) {
            ESP_LOGI(TAG, "[SAMPLE] T=%.2f°C, ADC=%d", temperature, adc_value);
        }
    }
}

