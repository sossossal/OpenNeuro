#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "config.h"
#include "zenoh_client.h"

static const char *TAG = "Zenoh-Client";

// 简化的 Zenoh 客户端实现（阶段一 MVP）
// 完整的 C 绑定需要编译 zenoh-pico 库

typedef struct {
    char topic[128];
    char payload[256];
    uint64_t timestamp;
} zenoh_message_t;

static zenoh_message_t g_last_servo_cmd = {
    .topic = "rt/device/esp32_001/servo/cmd",
    .payload = "{\"angle\": 0}",
    .timestamp = 0
};

/* ============= 模拟 Zenoh 消息处理 ============= */

void zenoh_client_task(void *arg) {
    ESP_LOGI(TAG, "Zenoh client task started");
    
    // 模拟连接过程
    ESP_LOGI(TAG, "Connecting to Zenoh router at %s...", ZENOH_ROUTER_LOCATOR);
    vTaskDelay(pdMS_TO_TICKS(1000));
    ESP_LOGI(TAG, "✓ Connected to Zenoh router");

    // 订阅舵机控制命令
    ESP_LOGI(TAG, "Subscribing to topic: rt/device/esp32_001/servo/cmd");

    // 发布设备状态
    ESP_LOGI(TAG, "Publishing device status every %d ms", ZENOH_HEARTBEAT_INTERVAL_MS);

    // 模拟主循环
    uint32_t counter = 0;
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(ZENOH_HEARTBEAT_INTERVAL_MS));

        // 构建状态消息
        char payload[256];
        snprintf(payload, sizeof(payload),
                "{\"device\": \"esp32_001\", \"uptime\": %u, \"status\": \"ok\"}",
                counter * ZENOH_HEARTBEAT_INTERVAL_MS);

        // 模拟发布
        ESP_LOGI(TAG, "[PUBLISH] rt/device/esp32_001/status: %s", payload);

        counter++;
        if (counter % 10 == 0) {
            ESP_LOGI(TAG, "[STATS] Heartbeat count: %u", counter);
        }
    }
}

void zenoh_publish_sensor(const char *topic, const char *payload) {
    ESP_LOGD(TAG, "[PUB] %s: %s", topic, payload);
    // 实际实现: z_publisher_put()
}

void zenoh_subscribe_handler(const char *topic, const char *payload) {
    ESP_LOGI(TAG, "[SUB] %s: %s", topic, payload);
    
    // 解析命令
    if (strstr(topic, "servo/cmd")) {
        // 处理舵机控制
        float angle = 0;
        if (sscanf(payload, "{\"angle\": %f", &angle) == 1) {
            ESP_LOGI(TAG, "Servo command received: angle = %.1f°", angle);
            // 传递给 servo_controller
        }
    } else if (strstr(topic, "led/cmd")) {
        // 处理 LED 控制
        ESP_LOGI(TAG, "LED command received");
    }
}

