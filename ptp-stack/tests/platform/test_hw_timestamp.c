#include "../platform/hw_timestamp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  const char *iface = "eth0";

  if (argc > 1) {
    iface = argv[1];
  }

  printf("=== Hardware Timestamp Test ===\n");
  printf("Interface: %s\n\n", iface);

  // 1. 检查支持
  printf("1. Checking hardware timestamp support...\n");
  if (hw_timestamp_is_supported(iface)) {
    printf("[PASS] Hardware timestamping supported on %s\n\n", iface);
  } else {
    printf("[FAIL] Hardware timestamping NOT supported on %s\n", iface);
    printf("Note: This may require:\n");
    printf("  - Linux kernel 3.17+\n");
    printf("  - Network card with PTP support (e.g., Intel I210, RTL8211FD)\n");
    printf("  - Root privileges\n\n");
    return 1;
  }

  // 2. 初始化
  printf("2. Initializing hardware timestamp...\n");
  hw_timestamp_ctx_t ctx;
  if (hw_timestamp_init(&ctx, iface) != 0) {
    printf("[FAIL] Initialization failed\n");
    return 1;
  }
  printf("[PASS] Initialized\n\n");

  // 3. 启用 TX 时间戳
  printf("3. Enabling TX timestamping...\n");
  if (hw_timestamp_enable_tx(&ctx) != 0) {
    printf("[WARN] TX timestamping failed (may need root)\n");
  } else {
    printf("[PASS] TX timestamping enabled\n");
  }
  printf("\n");

  // 4. 启用 RX 时间戳
  printf("4. Enabling RX timestamping...\n");
  if (hw_timestamp_enable_rx(&ctx) != 0) {
    printf("[WARN] RX timestamping failed (may need root)\n");
  } else {
    printf("[PASS] RX timestamping enabled\n");
  }
  printf("\n");

  // 5. 清理
  printf("5. Cleanup...\n");
  hw_timestamp_cleanup(&ctx);
  printf("[PASS] Cleaned up\n\n");

  printf("=== Test Complete ===\n");
  printf("If you see warnings, try running with sudo:\n");
  printf("  sudo ./test_hw_timestamp %s\n", iface);

  return 0;
}
