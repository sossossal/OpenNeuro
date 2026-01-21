#include "tsn_qos_bridge.h"
#include <stdio.h>

zenoh_priority_t tsn_to_zenoh_priority(tsn_traffic_class_t tsn_class) {
  // Mapping strategy:
  // TSN_CLASS_RT (Real-Time) → ZENOH_PRIORITY_REAL_TIME
  // TSN_CLASS_ST (Stream) → ZENOH_PRIORITY_INTERACTIVE
  // TSN_CLASS_CT (Control) → ZENOH_PRIORITY_DATA_HIGH
  // TSN_CLASS_BE (Best-Effort) → ZENOH_PRIORITY_BEST_EFFORT

  switch (tsn_class) {
  case TSN_CLASS_RT:
    return ZENOH_PRIORITY_REAL_TIME;
  case TSN_CLASS_ST:
    return ZENOH_PRIORITY_INTERACTIVE;
  case TSN_CLASS_CT:
    return ZENOH_PRIORITY_DATA_HIGH;
  case TSN_CLASS_BE:
  default:
    return ZENOH_PRIORITY_BEST_EFFORT;
  }
}

tsn_traffic_class_t zenoh_to_tsn_class(zenoh_priority_t zenoh_prio) {
  // Reverse mapping
  switch (zenoh_prio) {
  case ZENOH_PRIORITY_REAL_TIME:
    return TSN_CLASS_RT;
  case ZENOH_PRIORITY_INTERACTIVE:
  case ZENOH_PRIORITY_DATA_HIGH:
    return TSN_CLASS_ST;
  case ZENOH_PRIORITY_DATA:
  case ZENOH_PRIORITY_DATA_LOW:
    return TSN_CLASS_CT;
  case ZENOH_PRIORITY_BACKGROUND:
  case ZENOH_PRIORITY_BEST_EFFORT:
  default:
    return TSN_CLASS_BE;
  }
}

int tsn_get_congestion_control(tsn_traffic_class_t tsn_class) {
  // Real-Time and Stream traffic should BLOCK on congestion (reliable)
  // Control and Best-Effort can DROP (unreliable, latency-sensitive)

  switch (tsn_class) {
  case TSN_CLASS_RT:
  case TSN_CLASS_ST:
    return 1; // BLOCK (reliable delivery)
  case TSN_CLASS_CT:
  case TSN_CLASS_BE:
  default:
    return 0; // DROP (low latency)
  }
}

void tsn_qos_bridge_print_mapping(void) {
  printf("=== TSN to Zenoh QoS Mapping ===\n");
  printf("TSN_CLASS_RT → ZENOH_PRIORITY_REAL_TIME (1) [BLOCK]\n");
  printf("TSN_CLASS_ST → ZENOH_PRIORITY_INTERACTIVE (2) [BLOCK]\n");
  printf("TSN_CLASS_CT → ZENOH_PRIORITY_DATA_HIGH (3) [DROP]\n");
  printf("TSN_CLASS_BE → ZENOH_PRIORITY_BEST_EFFORT (7) [DROP]\n");
}
