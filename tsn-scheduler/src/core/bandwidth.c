#include "bandwidth.h"
#include <stdio.h>

// 假设总链路带宽 1Gbps
#define LINK_SPEED_BPS 1000000000ULL

// 各类别的最大保留百分比 (根据 design doc)
// RT: 40%, ST: 30%, CT: 15%, BE: 15%
static const uint32_t MAX_BW_LIMITS[4] = {
    400000000, // RT
    300000000, // ST
    150000000, // CT
    150000000  // BE
};

static uint32_t current_usage_bps[4] = {0, 0, 0, 0};

void tsn_bandwidth_init(void) {
  for (int i = 0; i < 4; i++)
    current_usage_bps[i] = 0;
}

bool tsn_bandwidth_check(tsn_traffic_class_t tc, uint32_t flow_bps) {
  if (tc > TSN_CLASS_BE)
    return false;

  uint32_t new_usage = current_usage_bps[tc] + flow_bps;

  if (new_usage <= MAX_BW_LIMITS[tc]) {
    return true;
  }

  printf("[TSN BW] Rejected flow (Class %d): Need %u bps, Avail %u bps\n", tc,
         flow_bps, MAX_BW_LIMITS[tc] - current_usage_bps[tc]);
  return false;
}

void tsn_bandwidth_reserve(tsn_traffic_class_t tc, uint32_t flow_bps) {
  if (tc <= TSN_CLASS_BE) {
    current_usage_bps[tc] += flow_bps;
    printf("[TSN BW] Reserved %u bps for Class %d. Total: %u bps\n", flow_bps,
           tc, current_usage_bps[tc]);
  }
}

void tsn_bandwidth_release(tsn_traffic_class_t tc, uint32_t flow_bps) {
  if (tc <= TSN_CLASS_BE) {
    if (current_usage_bps[tc] >= flow_bps) {
      current_usage_bps[tc] -= flow_bps;
    } else {
      current_usage_bps[tc] = 0; // Error / Underflow protection
    }
    printf("[TSN BW] Released %u bps for Class %d. Total: %u bps\n", flow_bps,
           tc, current_usage_bps[tc]);
  }
}

uint8_t tsn_bandwidth_get_usage(tsn_traffic_class_t tc) {
  if (tc > TSN_CLASS_BE)
    return 0;
  // return percentage
  return (uint8_t)((uint64_t)current_usage_bps[tc] * 100 / MAX_BW_LIMITS[tc]);
}
