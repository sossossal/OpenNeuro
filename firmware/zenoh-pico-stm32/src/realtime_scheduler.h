#ifndef REALTIME_SCHEDULER_H
#define REALTIME_SCHEDULER_H

#include <stdint.h>

void SysTick_Handler(void);
uint32_t scheduler_get_tick(void);
uint32_t scheduler_get_sensor_count(void);
uint32_t scheduler_get_zenoh_count(void);

#endif // REALTIME_SCHEDULER_H

