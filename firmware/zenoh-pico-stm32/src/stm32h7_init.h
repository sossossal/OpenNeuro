#ifndef STM32H7_INIT_H
#define STM32H7_INIT_H

#include <stdint.h>

void stm32h7_init(void);
void led_toggle(void);
void led_set(uint8_t state);

#endif // STM32H7_INIT_H

