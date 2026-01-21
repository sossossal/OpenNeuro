#ifndef TSN_QOS_BRIDGE_H
#define TSN_QOS_BRIDGE_H

#include "../../tsn-scheduler/src/core/queue.h"
#include <stdint.h>

// Zenoh Priority levels (from Zenoh spec)
typedef enum {
  ZENOH_PRIORITY_REAL_TIME = 1, // Highest priority
  ZENOH_PRIORITY_INTERACTIVE = 2,
  ZENOH_PRIORITY_DATA_HIGH = 3,
  ZENOH_PRIORITY_DATA = 4,
  ZENOH_PRIORITY_DATA_LOW = 5,
  ZENOH_PRIORITY_BACKGROUND = 6,
  ZENOH_PRIORITY_BEST_EFFORT = 7 // Lowest priority
} zenoh_priority_t;

// Map TSN traffic class to Zenoh priority
zenoh_priority_t tsn_to_zenoh_priority(tsn_traffic_class_t tsn_class);

// Map Zenoh priority to TSN traffic class (reverse)
tsn_traffic_class_t zenoh_to_tsn_class(zenoh_priority_t zenoh_prio);

// Get recommended Zenoh congestion control for TSN class
// Returns: 0 = DROP, 1 = BLOCK
int tsn_get_congestion_control(tsn_traffic_class_t tsn_class);

// Print QoS mapping table (for debugging)
void tsn_qos_bridge_print_mapping(void);

#endif // TSN_QOS_BRIDGE_H
