#include <stdio.h>
#include <string.h>
#include "config.h"
#include "zenoh_client.h"

static const char *TAG = "Zenoh";

// 模拟 Zenoh 客户端
volatile uint32_t zenoh_msg_count = 0;

void zenoh_init(void) {
    printf("[%s] Initializing Zenoh-Pico client\n", TAG);
    printf("[%s] Connecting to %s:%d...\n", TAG, ZENOH_ROUTER_IP, ZENOH_ROUTER_PORT);
}

void zenoh_publish(const char *resource, const uint8_t *payload, uint16_t length) {
    printf("[%s] PUBLISH %s: ", TAG, resource);
    for (uint16_t i = 0; i < length && i < 64; i++) {
        printf("%c", (char)payload[i]);
    }
    if (length > 64) printf("...");
    printf("\n");
    
    zenoh_msg_count++;
}

void zenoh_subscribe(const char *resource) {
    printf("[%s] SUBSCRIBE %s\n", TAG, resource);
}

uint32_t zenoh_get_msg_count(void) {
    return zenoh_msg_count;
}

