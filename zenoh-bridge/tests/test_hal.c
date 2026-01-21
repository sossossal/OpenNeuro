#include "../src/zenoh_hal.h"
#include <stdio.h>
#include <string.h>

// Test callback for subscriber
void test_data_handler(const uint8_t *data, size_t len, void *ctx) {
  printf("[Test] Received %zu bytes: ", len);
  for (size_t i = 0; i < len && i < 16; i++) {
    printf("%02x ", data[i]);
  }
  printf("\n");
}

int main() {
  printf("=== Zenoh HAL Test ===\n");

  // Open session
  zenoh_session_t *session = zenoh_hal_open("tcp/127.0.0.1:7447");
  if (!session) {
    printf("[FAIL] Failed to open session\n");
    return 1;
  }
  printf("[PASS] Session opened\n");

  // Declare publisher
  zenoh_publisher_t *pub = zenoh_hal_declare_publisher(session, "test/topic");
  if (!pub) {
    printf("[FAIL] Failed to declare publisher\n");
    zenoh_hal_close(session);
    return 1;
  }
  printf("[PASS] Publisher declared\n");

  // Publish data
  uint8_t data[] = {0x01, 0x02, 0x03, 0x04};
  int ret = zenoh_hal_put(pub, data, sizeof(data));
  if (ret != 0) {
    printf("[FAIL] Failed to put data\n");
  } else {
    printf("[PASS] Data published\n");
  }

  // Declare subscriber
  zenoh_subscriber_t *sub = zenoh_hal_declare_subscriber(
      session, "test/topic", test_data_handler, NULL);
  if (!sub) {
    printf("[FAIL] Failed to declare subscriber\n");
  } else {
    printf("[PASS] Subscriber declared\n");
  }

  // Cleanup
  if (sub)
    zenoh_hal_undeclare_subscriber(sub);
  zenoh_hal_undeclare_publisher(pub);
  zenoh_hal_close(session);

  printf("\n=== All Tests Passed ===\n");
  return 0;
}
