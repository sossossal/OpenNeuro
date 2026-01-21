#include "zenoh_hal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#ifdef USE_REAL_ZENOH
#include <zenoh.h>

// 真实 Zenoh 会话结构
struct zenoh_session_s {
  z_owned_session_t z_session;
  char locator[256];
};

// 真实 Zenoh 发布器结构
struct zenoh_publisher_s {
  z_owned_publisher_t z_pub;
  z_owned_keyexpr_t keyexpr;
  char key[256];
};

// 真实 Zenoh 订阅器结构
struct zenoh_subscriber_s {
  z_owned_subscriber_t z_sub;
  z_owned_keyexpr_t keyexpr;
  zenoh_data_handler_t handler;
  void *user_ctx;
  char key[256];
};

// 打开 Zenoh 会话
zenoh_session_t *zenoh_hal_open(const char *locator) {
  zenoh_session_t *s = (zenoh_session_t *)malloc(sizeof(zenoh_session_t));
  if (!s)
    return NULL;

  memset(s, 0, sizeof(zenoh_session_t));
  if (locator) {
    strncpy(s->locator, locator, sizeof(s->locator) - 1);
  }

  // 创建配置
  z_owned_config_t config = z_config_default();

  if (locator && strlen(locator) > 0) {
    // 配置连接地址
    zp_config_insert(z_loan(config), Z_CONFIG_CONNECT_KEY,
                     z_string_make(locator));
  }

  // 打开会话
  s->z_session = z_open(z_move(config));

  if (!z_check(s->z_session)) {
    fprintf(stderr, "[Zenoh HAL] Failed to open session\n");
    free(s);
    return NULL;
  }

  printf("[Zenoh HAL Real] Session opened: %s\n",
         locator ? locator : "default");

  return s;
}

// 关闭 Zenoh 会话
void zenoh_hal_close(zenoh_session_t *session) {
  if (session) {
    z_close(z_move(session->z_session));
    free(session);
    printf("[Zenoh HAL Real] Session closed\n");
  }
}

// 声明发布器
zenoh_publisher_t *zenoh_hal_declare_publisher(zenoh_session_t *s,
                                               const char *key) {
  if (!s || !key)
    return NULL;

  zenoh_publisher_t *pub =
      (zenoh_publisher_t *)malloc(sizeof(zenoh_publisher_t));
  if (!pub)
    return NULL;

  memset(pub, 0, sizeof(zenoh_publisher_t));
  strncpy(pub->key, key, sizeof(pub->key) - 1);

  // 创建 keyexpr
  pub->keyexpr = z_keyexpr(key);

  // 声明发布器
  pub->z_pub =
      z_declare_publisher(z_loan(s->z_session), z_loan(pub->keyexpr), NULL);

  if (!z_check(pub->z_pub)) {
    fprintf(stderr, "[Zenoh HAL] Failed to declare publisher\n");
    z_drop(z_move(pub->keyexpr));
    free(pub);
    return NULL;
  }

  printf("[Zenoh HAL Real] Publisher declared: %s\n", key);
  return pub;
}

// 发布数据
int zenoh_hal_put(zenoh_publisher_t *pub, const uint8_t *data, size_t len) {
  if (!pub || !data)
    return -1;

  // 发布数据
  z_publisher_put_options_t options = z_publisher_put_options_default();

  int ret = z_publisher_put(z_loan(pub->z_pub), data, len, &options);

  if (ret < 0) {
    fprintf(stderr, "[Zenoh HAL] Put failed\n");
    return -1;
  }

  printf("[Zenoh HAL Real] Put %zu bytes to '%s'\n", len, pub->key);
  return 0;
}

// 取消发布器
void zenoh_hal_undeclare_publisher(zenoh_publisher_t *pub) {
  if (pub) {
    z_undeclare_publisher(z_move(pub->z_pub));
    z_drop(z_move(pub->keyexpr));
    free(pub);
    printf("[Zenoh HAL Real] Publisher undeclared\n");
  }
}

// Zenoh 数据回调包装
static void zenoh_data_callback_wrapper(const z_sample_t *sample, void *arg) {
  zenoh_subscriber_t *sub = (zenoh_subscriber_t *)arg;

  if (sub && sub->handler) {
    // 提取数据
    z_bytes_t payload = z_sample_payload(sample);
    size_t len = z_bytes_len(&payload);

    // 分配临时缓冲区
    uint8_t *data = (uint8_t *)malloc(len);
    if (data) {
      z_bytes_to_buf(&payload, data, len);

      // 调用用户回调
      sub->handler(data, len, sub->user_ctx);

      free(data);
    }
  }
}

// 声明订阅器
zenoh_subscriber_t *zenoh_hal_declare_subscriber(zenoh_session_t *s,
                                                 const char *key,
                                                 zenoh_data_handler_t handler,
                                                 void *ctx) {
  if (!s || !key || !handler)
    return NULL;

  zenoh_subscriber_t *sub =
      (zenoh_subscriber_t *)malloc(sizeof(zenoh_subscriber_t));
  if (!sub)
    return NULL;

  memset(sub, 0, sizeof(zenoh_subscriber_t));
  strncpy(sub->key, key, sizeof(sub->key) - 1);
  sub->handler = handler;
  sub->user_ctx = ctx;

  // 创建 keyexpr
  sub->keyexpr = z_keyexpr(key);

  // 创建回调闭包
  z_owned_closure_sample_t callback =
      z_closure(zenoh_data_callback_wrapper, NULL, sub);

  // 声明订阅器
  sub->z_sub = z_declare_subscriber(z_loan(s->z_session), z_loan(sub->keyexpr),
                                    z_move(callback), NULL);

  if (!z_check(sub->z_sub)) {
    fprintf(stderr, "[Zenoh HAL] Failed to declare subscriber\n");
    z_drop(z_move(sub->keyexpr));
    free(sub);
    return NULL;
  }

  printf("[Zenoh HAL Real] Subscriber declared: %s\n", key);
  return sub;
}

// 取消订阅器
void zenoh_hal_undeclare_subscriber(zenoh_subscriber_t *sub) {
  if (sub) {
    z_undeclare_subscriber(z_move(sub->z_sub));
    z_drop(z_move(sub->keyexpr));
    free(sub);
    printf("[Zenoh HAL Real] Subscriber undeclared\n");
  }
}

#else
// Mock 实现 (保持原有代码)
#include "zenoh_hal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct zenoh_session_s {
  void *mock_session;
  char locator[256];
};

struct zenoh_publisher_s {
  void *mock_pub;
  char key[256];
};

struct zenoh_subscriber_s {
  void *mock_sub;
  zenoh_data_handler_t handler;
  void *user_ctx;
  char key[256];
};

zenoh_session_t *zenoh_hal_open(const char *locator) {
  zenoh_session_t *s = (zenoh_session_t *)malloc(sizeof(zenoh_session_t));
  if (!s)
    return NULL;

  memset(s, 0, sizeof(zenoh_session_t));
  if (locator) {
    strncpy(s->locator, locator, sizeof(s->locator) - 1);
  }

  s->mock_session = (void *)0x12345678;
  printf("[Zenoh HAL Mock] Session opened: %s\n",
         locator ? locator : "default");

  return s;
}

void zenoh_hal_close(zenoh_session_t *session) {
  if (session) {
    free(session);
    printf("[Zenoh HAL Mock] Session closed\n");
  }
}

zenoh_publisher_t *zenoh_hal_declare_publisher(zenoh_session_t *s,
                                               const char *key) {
  if (!s || !key)
    return NULL;

  zenoh_publisher_t *pub =
      (zenoh_publisher_t *)malloc(sizeof(zenoh_publisher_t));
  if (!pub)
    return NULL;

  memset(pub, 0, sizeof(zenoh_publisher_t));
  strncpy(pub->key, key, sizeof(pub->key) - 1);
  pub->mock_pub = (void *)0x87654321;

  printf("[Zenoh HAL Mock] Publisher declared: %s\n", key);
  return pub;
}

int zenoh_hal_put(zenoh_publisher_t *pub, const uint8_t *data, size_t len) {
  if (!pub || !data)
    return -1;

  printf("[Zenoh HAL Mock] Put %zu bytes to '%s'\n", len, pub->key);
  return 0;
}

void zenoh_hal_undeclare_publisher(zenoh_publisher_t *pub) {
  if (pub) {
    free(pub);
    printf("[Zenoh HAL Mock] Publisher undeclared\n");
  }
}

zenoh_subscriber_t *zenoh_hal_declare_subscriber(zenoh_session_t *s,
                                                 const char *key,
                                                 zenoh_data_handler_t handler,
                                                 void *ctx) {
  if (!s || !key || !handler)
    return NULL;

  zenoh_subscriber_t *sub =
      (zenoh_subscriber_t *)malloc(sizeof(zenoh_subscriber_t));
  if (!sub)
    return NULL;

  memset(sub, 0, sizeof(zenoh_subscriber_t));
  strncpy(sub->key, key, sizeof(sub->key) - 1);
  sub->handler = handler;
  sub->user_ctx = ctx;
  sub->mock_sub = (void *)0x11223344;

  printf("[Zenoh HAL Mock] Subscriber declared: %s\n", key);
  return sub;
}

void zenoh_hal_undeclare_subscriber(zenoh_subscriber_t *sub) {
  if (sub) {
    free(sub);
    printf("[Zenoh HAL Mock] Subscriber undeclared\n");
  }
}

#endif
