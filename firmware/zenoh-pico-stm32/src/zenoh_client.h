#ifndef ZENOH_CLIENT_H
#define ZENOH_CLIENT_H

#include <stdint.h>

void zenoh_init(void);
void zenoh_publish(const char *resource, const uint8_t *payload, uint16_t length);
void zenoh_subscribe(const char *resource);
uint32_t zenoh_get_msg_count(void);

#endif // ZENOH_CLIENT_H

