#include <stdint.h>
#include <stdio.h>


// Platform Abstraction Layer Stub

int platform_init(void) {
  printf("[Platform] Init (Stub)\n");
  return 0;
}

uint64_t platform_get_nanoseconds(void) {
  // Return dummy time for now
  return 0;
}

int platform_tx_packet(uint8_t *data, size_t len) {
  (void)data;
  (void)len;
  return 0; // Success
}
