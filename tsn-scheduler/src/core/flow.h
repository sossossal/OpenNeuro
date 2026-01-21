#ifndef TSN_FLOW_H
#define TSN_FLOW_H

#include "../core/queue.h" // For traffic classes
#include <stdbool.h>
#include <stdint.h>


#define TSN_MAX_FLOWS 256

// Flow Identification
typedef struct {
  uint32_t src_ip;
  uint32_t dst_ip;
  uint16_t src_port;
  uint16_t dst_port;
  uint8_t protocol;      // UDP/TCP
  uint8_t vlan_priority; // PCP
} tsn_flow_id_t;

// Flow Properties
typedef struct {
  uint32_t flow_id;
  tsn_traffic_class_t traffic_class; // Mapped class
  uint32_t max_frame_size;
  uint32_t period_us;
  // Bandwidth Usage (Internal)
  uint32_t bandwidth_bps;
} tsn_flow_entry_t;

// API
int tsn_flow_table_init(void);

// Add a flow - Returns 0 on success
int tsn_flow_add(const tsn_flow_id_t *id, const tsn_flow_entry_t *props);

// Remove a flow
int tsn_flow_remove(uint32_t flow_id);

// Find flow by ID
tsn_flow_entry_t *tsn_flow_get(uint32_t flow_id);

// Match packet to flow (Classification) - Returns Flow ID or 0 if no match
uint32_t tsn_flow_classify(const tsn_flow_id_t *packet_header);

#endif // TSN_FLOW_H
