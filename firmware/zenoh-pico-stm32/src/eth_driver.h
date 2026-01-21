#ifndef ETH_DRIVER_H
#define ETH_DRIVER_H

#include <stdint.h>

void eth_init(void);
void eth_send_packet(const uint8_t *data, uint16_t length);
void eth_receive_packet(uint8_t *buffer, uint16_t max_length);
int eth_link_is_up(void);
void eth_set_link_status(int up);

#endif // ETH_DRIVER_H

