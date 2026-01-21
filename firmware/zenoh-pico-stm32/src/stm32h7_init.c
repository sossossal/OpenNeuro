#include <stdio.h>
#include "stm32h7xx.h"
#include "config.h"

static void clock_init(void) {
    // 启用 Ethernet 接口
    RCC->AHB1ENR |= RCC_AHB1ENR_ETHMACEN | RCC_AHB1ENR_ETHMACTXEN | RCC_AHB1ENR_ETHMACRXEN;
    
    // 启用 GPIO 时钟
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN | RCC_AHB4ENR_GPIOBEN | RCC_AHB4ENR_GPIOCEN;
    
    // 启用 ADC 时钟
    RCC->AHB1ENR |= RCC_AHB1ENR_ADC12EN;
    
    // 启用 TIM1（PWM）
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
}

static void gpio_init(void) {
    // LED 输出
    GPIOB->MODER |= (1U << (LED_PIN * 2));
    GPIOB->MODER &= ~(1U << (LED_PIN * 2 + 1));
    
    // 状态 LED
    GPIOB->MODER |= (1U << (STATUS_LED_PIN * 2));
    GPIOB->MODER &= ~(1U << (STATUS_LED_PIN * 2 + 1));
}

static void systick_init(void) {
    // 配置 SysTick 中断（1ms）
    SysTick_Config(SystemCoreClock / 1000);
}

void stm32h7_init(void) {
    // 更新系统时钟
    SystemInit();
    
    // 初始化各个模块
    clock_init();
    gpio_init();
    systick_init();
    
    // LED 亮起表示启动完成
    GPIOB->BSRR = (1U << LED_PIN);
}

void led_toggle(void) {
    GPIOB->ODRBIT[LED_PIN] ^= 1;
}

void led_set(uint8_t state) {
    if (state) {
        GPIOB->BSRR = (1U << LED_PIN);
    } else {
        GPIOB->BSRR = (1U << (LED_PIN + 16));
    }
}

