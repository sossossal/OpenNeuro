#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_log.h"

#include "config.h"

static const char *TAG = "Servo-Controller";

// PWM 配置
#define SERVO_MIN_PULSE 500    // 0 度对应 500µs
#define SERVO_MAX_PULSE 2500   // 180 度对应 2500µs
#define SERVO_FREQ 50          // 50Hz (20ms 周期)
#define SERVO_RESOLUTION LEDC_TIMER_12_BIT  // 4096 个单位

void servo_init(void) {
    ledc_timer_config_t timer_conf = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = SERVO_RESOLUTION,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = SERVO_FREQ,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer_conf);

    ledc_channel_config_t channel_conf = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = SERVO_GPIO,
        .duty = 0,
        .hpoint = 0
    };
    ledc_channel_config(&channel_conf);

    ESP_LOGI(TAG, "Servo PWM initialized: GPIO %d, Frequency %d Hz", SERVO_GPIO, SERVO_FREQ);
}

void servo_set_angle(float angle) {
    if (angle < 0) angle = 0;
    if (angle > 180) angle = 180;

    // 计算 PWM duty（0.5ms ~ 2.5ms 脉宽）
    uint32_t pulse_us = SERVO_MIN_PULSE + (angle / 180.0) * (SERVO_MAX_PULSE - SERVO_MIN_PULSE);
    
    // 转换为 duty 值（周期 20ms = 20000µs）
    uint32_t duty = (pulse_us * (1 << LEDC_TIMER_12_BIT)) / 20000;

    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);

    ESP_LOGI(TAG, "Servo angle set to %.1f° (PWM duty: %u)", angle, duty);
}

float servo_get_angle(void) {
    uint32_t duty = ledc_get_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    uint32_t pulse_us = (duty * 20000) / (1 << LEDC_TIMER_12_BIT);
    float angle = ((float)(pulse_us - SERVO_MIN_PULSE) / (SERVO_MAX_PULSE - SERVO_MIN_PULSE)) * 180.0;
    return angle;
}

