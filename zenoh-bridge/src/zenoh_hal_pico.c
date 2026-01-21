#include "zenoh_hal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Zenoh-Pico implementation (for ESP32, STM32, etc.)
// This would normally include <zenoh-pico.h>

// Mock structures (replace with real Zenoh-Pico types)
struct zenoh_session_s {
  char locator[256];
  int dummy;
};

struct zenoh_publisher_s {
  zenoh_session_t *session;
  char key[128];
};

struct zenoh_subscriber_s {
  zenoh_session_t *session;
  char key[128];
  zenoh_data_handler_t handler;
  void *ctx;
};

zenoh_session_t *zenoh_hal_open(const char *locator) {
  zenoh_session_t *s = (zenoh_session_t *)malloc(sizeof(zenoh_session_t));
  if (!s)
    return NULL;

  strncpy(s->locator, locator ? locator : "tcp/127.0.0.1:7447", 255);
  s->dummy = 0;

  printf("[Zenoh HAL Pico] Session opened: %s\n", s->locator);
  return s;
}

void zenoh_hal_close(zenoh_session_t *session) {
  if (session) {
    printf("[Zenoh HAL Pico] Session closed\n");
    free(session);
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

  pub->session = s;
  strncpy(pub->key, key, 127);

  printf("[Zenoh HAL Pico] Publisher declared: %s\n", key);
  return pub;
}

int zenoh_hal_put(zenoh_publisher_t *pub, const uint8_t *data, size_t len) {
  if (!pub || !data)
    return -1;

  printf("[Zenoh HAL Pico] Put %zu bytes to '%s'\n", len, pub->key);
  // Real implementation: zp_put(pub->zp_pub, data, len);
  return 0;
}

void zenoh_hal_undeclare_publisher(zenoh_publisher_t *pub) {
  if (pub) {
    printf("[Zenoh HAL Pico] Publisher undeclared: %s\n", pub->key);
    free(pub);
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

  sub->session = s;
  strncpy(sub->key, key, 127);
  sub->handler = handler;
  sub->ctx = ctx;

  printf("[Zenoh HAL Pico] Subscriber declared: %s\n", key);
  return sub;
}

void zenoh_hal_undeclare_subscriber(zenoh_subscriber_t *sub) {
  if (sub) {
    printf("[Zenoh HAL Pico] Subscriber undeclared: %s\n", sub->key);
    free(sub);
  }
}
