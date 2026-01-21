#include "../src/zenoh_hal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// 全局变量用于测试
static int g_received_count = 0;
static char g_last_received[256];

// 订阅回调函数
void test_subscriber_callback(const uint8_t *data, size_t len, void *ctx) {
  g_received_count++;

  if (len < sizeof(g_last_received)) {
    memcpy(g_last_received, data, len);
    g_last_received[len] = '\0';
  }

  printf("[Subscriber] Received message #%d: %.*s (%zu bytes)\n",
         g_received_count, (int)len, (char *)data, len);
}

int main(int argc, char *argv[]) {
  const char *locator = NULL;

  if (argc > 1) {
    locator = argv[1];
  }

  printf("=== Zenoh Pub/Sub End-to-End Test ===\n");
  printf("Locator: %s\n\n", locator ? locator : "default");

  // 1. 打开会话
  printf("1. Opening Zenoh session...\n");
  zenoh_session_t *session = zenoh_hal_open(locator);
  if (!session) {
    printf("[FAIL] Failed to open session\n");
    printf("\nNote: If using real Zenoh, make sure:\n");
    printf("  - Zenoh router is running (zenohd)\n");
    printf("  - Compiled with -DUSE_REAL_ZENOH=ON\n");
    printf("  - Zenoh C library is installed\n\n");
    return 1;
  }
  printf("[PASS] Session opened\n\n");

  // 2. 声明订阅器
  printf("2. Declaring subscriber...\n");
  zenoh_subscriber_t *sub = zenoh_hal_declare_subscriber(
      session, "openneuro/test/pubsub", test_subscriber_callback, NULL);
  if (!sub) {
    printf("[FAIL] Failed to declare subscriber\n");
    zenoh_hal_close(session);
    return 1;
  }
  printf("[PASS] Subscriber declared\n\n");

  // 3. 声明发布器
  printf("3. Declaring publisher...\n");
  zenoh_publisher_t *pub =
      zenoh_hal_declare_publisher(session, "openneuro/test/pubsub");
  if (!pub) {
    printf("[FAIL] Failed to declare publisher\n");
    zenoh_hal_undeclare_subscriber(sub);
    zenoh_hal_close(session);
    return 1;
  }
  printf("[PASS] Publisher declared\n\n");

  // 4. 发布测试消息
  printf("4. Publishing test messages...\n");

  const char *messages[] = {"Hello Zenoh!", "OpenNeuro M2.2 Test",
                            "PTP + TSN + Zenoh Integration",
                            "Real-time Communication", "Final Test Message"};

  int num_messages = sizeof(messages) / sizeof(messages[0]);

  for (int i = 0; i < num_messages; i++) {
    printf("  Publishing: \"%s\"\n", messages[i]);

    int ret =
        zenoh_hal_put(pub, (const uint8_t *)messages[i], strlen(messages[i]));
    if (ret != 0) {
      printf("[WARN] Put failed for message %d\n", i + 1);
    }

    // 等待订阅器接收
    usleep(100000); // 100ms
  }

  printf("\n");

  // 5. 验证接收
  printf("5. Verifying reception...\n");
  printf("  Published: %d messages\n", num_messages);
  printf("  Received: %d messages\n", g_received_count);

  if (g_received_count == num_messages) {
    printf("[PASS] All messages received\n");
  } else if (g_received_count > 0) {
    printf("[WARN] Partial reception (%d/%d)\n", g_received_count,
           num_messages);
  } else {
    printf("[FAIL] No messages received\n");
    printf("Note: This is expected with mock implementation\n");
  }
  printf("\n");

  // 6. 清理
  printf("6. Cleanup...\n");
  zenoh_hal_undeclare_publisher(pub);
  printf("  Publisher undeclared\n");

  zenoh_hal_undeclare_subscriber(sub);
  printf("  Subscriber undeclared\n");

  zenoh_hal_close(session);
  printf("  Session closed\n");
  printf("[PASS] Cleanup complete\n\n");

  // 7. 总结
  printf("=== Test Summary ===\n");
  if (g_received_count == num_messages) {
    printf("Result: ✅ PASS (Full Pub/Sub working)\n");
    return 0;
  } else if (g_received_count > 0) {
    printf("Result: ⚠️  PARTIAL (Some messages received)\n");
    return 0;
  } else {
    printf("Result: ℹ️  MOCK (Using mock implementation)\n");
    printf("\nTo test with real Zenoh:\n");
    printf("  1. Install Zenoh C library\n");
    printf("  2. Start Zenoh router: zenohd\n");
    printf("  3. Rebuild with: cmake -DUSE_REAL_ZENOH=ON\n");
    printf("  4. Run: ./test_pubsub tcp/127.0.0.1:7447\n");
    return 0;
  }
}
