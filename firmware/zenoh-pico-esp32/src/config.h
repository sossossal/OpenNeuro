#ifndef ESP32_ZENOH_CONFIG_H
#define ESP32_ZENOH_CONFIG_H

#include <stdint.h>

/* ============= WiFi Configuration ============= */
#define WIFI_SSID CONFIG_WIFI_SSID
#define WIFI_PASSWORD CONFIG_WIFI_PASSWORD
#define WIFI_MAX_RETRIES 5
#define WIFI_TIMEOUT_MS 10000

/* ============= Zenoh Configuration ============= */
#define ZENOH_ROUTER_LOCATOR CONFIG_ZENOH_ROUTER_IP ":" CONFIG_ZENOH_ROUTER_PORT
#define ZENOH_PICO_SESSION_TIMEOUT_MS 3000
#define ZENOH_BUFFER_SIZE 256

/* ============= Device Configuration ============= */
#define DEVICE_NAME CONFIG_DEVICE_NAME
#define DEVICE_MAC_ADDR_LEN 6

/* ============= GPIO Configuration ============= */
#define LED_GPIO 2
#define SERVO_GPIO 18
#define BUTTON_GPIO 0

/* ============= ADC Configuration (Sensor) ============= */
#define SENSOR_ADC_PIN ADC1_CHANNEL_0  // GPIO 36 on ESP32-S3
#define SENSOR_RATE CONFIG_SENSOR_RATE  // Hz
#define SENSOR_SAMPLES 10

/* ============= Zenoh Topics ============= */
#define TOPIC_SERVO_CMD "rt/device/%s/servo/cmd"
#define TOPIC_LED_CMD "rt/device/%s/led/cmd"
#define TOPIC_SENSOR_TEMP "rt/device/%s/sensor/temperature"
#define TOPIC_SENSOR_ADC "rt/device/%s/sensor/adc"
#define TOPIC_DEVICE_STATUS "rt/device/%s/status"

/* ============= Task Configuration ============= */
#define TASK_PRIORITY_ZENOH 4
#define TASK_PRIORITY_SENSOR 3
#define TASK_STACK_SIZE_ZENOH 4096
#define TASK_STACK_SIZE_SENSOR 2048

/* ============= Logging ============= */
#define LOG_LEVEL CONFIG_LOG_LEVEL
#define LOG_PREFIX "[ESP32-Zenoh]"

/* ============= Performance Tuning ============= */
#define SENSOR_READ_INTERVAL_MS (1000 / CONFIG_SENSOR_RATE)
#define ZENOH_HEARTBEAT_INTERVAL_MS 1000

/* ============= Message Formats ============= */
typedef struct {
    float angle;
    uint16_t speed;
    uint16_t acceleration;
} servo_cmd_t;

typedef struct {
    uint8_t on;
    uint8_t brightness;
    uint32_t color;
} led_cmd_t;

typedef struct {
    float temperature;
    float humidity;
    uint32_t timestamp_ms;
} sensor_data_t;

#endif // ESP32_ZENOH_CONFIG_H

