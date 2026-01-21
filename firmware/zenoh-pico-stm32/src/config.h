#ifndef STM32H7_CONFIG_H
#define STM32H7_CONFIG_H

#include <stdint.h>

/* ============= Ethernet Configuration ============= */
#define ETH_MAC_ADDR {0x00, 0x00, 0x00, 0x00, 0x00, 0x01}
#define IP_ADDRESS "192.168.1.100"
#define GATEWAY_ADDRESS "192.168.1.1"
#define NETMASK "255.255.255.0"

/* ============= Zenoh Configuration ============= */
#define ZENOH_ROUTER_IP "192.168.1.1"
#define ZENOH_ROUTER_PORT 7447
#define ZENOH_PICO_SESSION_TIMEOUT_MS 3000

/* ============= Device Configuration ============= */
#define DEVICE_NAME "stm32h7_motor_controller"
#define DEVICE_ID 0x0001

/* ============= GPIO Configuration ============= */
#define LED_PORT GPIOB
#define LED_PIN GPIO_PIN_0
#define STATUS_LED_PORT GPIOB
#define STATUS_LED_PIN GPIO_PIN_1

/* ============= ADC Configuration (Sensor) ============= */
#define SENSOR_ADC ADC1
#define SENSOR_CHANNEL ADC_CHANNEL_0  // PA0
#define SENSOR_RATE 1000  // 1kHz

/* ============= PWM Configuration ============= */
#define MOTOR_PWM_TIMER TIM1
#define MOTOR_PWM_CHANNEL TIM_CHANNEL_1  // PA8
#define MOTOR_PWM_FREQ 20000  // 20kHz

/* ============= Zenoh Topics ============= */
#define TOPIC_MOTOR_CMD "rt/device/%s/motor/cmd"
#define TOPIC_SENSOR_CURRENT "rt/device/%s/sensor/current"
#define TOPIC_DEVICE_STATUS "rt/device/%s/status"

/* ============= PTP Configuration (Stage 2) ============= */
#define PTP_ENABLED 0  // 阶段二启用
#define PTP_SLAVE_MODE 1

/* ============= Performance Tuning ============= */
#define SENSOR_READ_INTERVAL_MS (1000 / SENSOR_RATE)
#define ZENOH_HEARTBEAT_INTERVAL_MS 1000

/* ============= Message Types ============= */
typedef struct {
    float pwm_duty;
    uint16_t direction;
    uint32_t ramp_time;
} motor_cmd_t;

typedef struct {
    float current_ma;
    float voltage_mv;
    uint32_t timestamp_us;
} sensor_data_t;

#endif // STM32H7_CONFIG_H

