#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "driver/pwm.h"
#include "driver/adc.h"

#include "config.h"
#include "wifi_config.h"
#include "zenoh_client.h"
#include "sensor_driver.h"
#include "servo_controller.h"

static const char *TAG = "ESP32-Zenoh-Main";

void app_main(void) {
    ESP_LOGI(TAG, "=== OpenNeuro ESP32 Zenoh-Pico Demo v0.1.0 ===");
    ESP_LOGI(TAG, "Device: %s", DEVICE_NAME);

    /* ========== 1. 初始化 NVS 内存 ========== */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "✓ NVS Flash initialized");

    /* ========== 2. 初始化网络栈 ========== */
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_LOGI(TAG, "✓ Network stack initialized");

    /* ========== 3. WiFi 连接 ========== */
    wifi_init_sta();
    ESP_LOGI(TAG, "✓ WiFi initialized");

    /* ========== 4. 初始化外设 ========== */
    // LED
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << LED_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
    gpio_set_level(LED_GPIO, 0);
    ESP_LOGI(TAG, "✓ LED initialized on GPIO %d", LED_GPIO);

    // 舵机 PWM
    servo_init();
    ESP_LOGI(TAG, "✓ Servo initialized on GPIO %d", SERVO_GPIO);

    // ADC 传感器
    sensor_init();
    ESP_LOGI(TAG, "✓ Sensor ADC initialized");

    /* ========== 5. 启动 Zenoh 客户端 ========== */
    xTaskCreate(zenoh_client_task, "zenoh_task", TASK_STACK_SIZE_ZENOH, NULL, 
                TASK_PRIORITY_ZENOH, NULL);
    ESP_LOGI(TAG, "✓ Zenoh client task started");

    /* ========== 6. 启动传感器采集任务 ========== */
    xTaskCreate(sensor_task, "sensor_task", TASK_STACK_SIZE_SENSOR, NULL, 
                TASK_PRIORITY_SENSOR, NULL);
    ESP_LOGI(TAG, "✓ Sensor task started");

    /* ========== 7. 心跳指示 ========== */
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(2000));
        gpio_set_level(LED_GPIO, 1);
        vTaskDelay(pdMS_TO_TICKS(100));
        gpio_set_level(LED_GPIO, 0);
        ESP_LOGI(TAG, "[HEARTBEAT] Device running normally");
    }
}

