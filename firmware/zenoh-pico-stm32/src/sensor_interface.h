#ifndef SENSOR_INTERFACE_H
#define SENSOR_INTERFACE_H

#include <stdint.h>
#include "config.h"

void sensor_init(void);
uint16_t sensor_read_adc(void);
float sensor_read_voltage(void);
void sensor_read_all(sensor_data_t *data);

#endif // SENSOR_INTERFACE_H

