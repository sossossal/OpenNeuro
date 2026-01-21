#include "../src/tsn_qos_bridge.h"
#include <assert.h>
#include <stdio.h>


int main() {
  printf("=== TSN to Zenoh QoS Mapping Test ===\n\n");

  // Print mapping table
  tsn_qos_bridge_print_mapping();
  printf("\n");

  // Test TSN → Zenoh mapping
  printf("Testing TSN → Zenoh Priority Mapping:\n");

  zenoh_priority_t rt_prio = tsn_to_zenoh_priority(TSN_CLASS_RT);
  assert(rt_prio == ZENOH_PRIORITY_REAL_TIME);
  printf("[PASS] TSN_CLASS_RT → ZENOH_PRIORITY_REAL_TIME (%d)\n", rt_prio);

  zenoh_priority_t st_prio = tsn_to_zenoh_priority(TSN_CLASS_ST);
  assert(st_prio == ZENOH_PRIORITY_INTERACTIVE);
  printf("[PASS] TSN_CLASS_ST → ZENOH_PRIORITY_INTERACTIVE (%d)\n", st_prio);

  zenoh_priority_t ct_prio = tsn_to_zenoh_priority(TSN_CLASS_CT);
  assert(ct_prio == ZENOH_PRIORITY_DATA_HIGH);
  printf("[PASS] TSN_CLASS_CT → ZENOH_PRIORITY_DATA_HIGH (%d)\n", ct_prio);

  zenoh_priority_t be_prio = tsn_to_zenoh_priority(TSN_CLASS_BE);
  assert(be_prio == ZENOH_PRIORITY_BEST_EFFORT);
  printf("[PASS] TSN_CLASS_BE → ZENOH_PRIORITY_BEST_EFFORT (%d)\n", be_prio);

  // Test Zenoh → TSN reverse mapping
  printf("\nTesting Zenoh → TSN Traffic Class Mapping:\n");

  tsn_traffic_class_t rt_class = zenoh_to_tsn_class(ZENOH_PRIORITY_REAL_TIME);
  assert(rt_class == TSN_CLASS_RT);
  printf("[PASS] ZENOH_PRIORITY_REAL_TIME → TSN_CLASS_RT\n");

  tsn_traffic_class_t be_class = zenoh_to_tsn_class(ZENOH_PRIORITY_BEST_EFFORT);
  assert(be_class == TSN_CLASS_BE);
  printf("[PASS] ZENOH_PRIORITY_BEST_EFFORT → TSN_CLASS_BE\n");

  // Test congestion control
  printf("\nTesting Congestion Control Policy:\n");

  int rt_cc = tsn_get_congestion_control(TSN_CLASS_RT);
  assert(rt_cc == 1); // BLOCK
  printf("[PASS] TSN_CLASS_RT → BLOCK (reliable)\n");

  int be_cc = tsn_get_congestion_control(TSN_CLASS_BE);
  assert(be_cc == 0); // DROP
  printf("[PASS] TSN_CLASS_BE → DROP (low latency)\n");

  printf("\n=== All QoS Mapping Tests Passed ===\n");
  return 0;
}
