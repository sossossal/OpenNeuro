#include "flow.h"
#include <stdio.h>
#include <string.h>


typedef struct {
  tsn_flow_id_t id;
  tsn_flow_entry_t entry;
  bool active;
} tsn_flow_slot_t;

static tsn_flow_slot_t g_flow_table[TSN_MAX_FLOWS];

int tsn_flow_table_init(void) {
  memset(g_flow_table, 0, sizeof(g_flow_table));
  return 0;
}

int tsn_flow_add(const tsn_flow_id_t *id, const tsn_flow_entry_t *props) {
  // 1. Find empty slot
  for (int i = 0; i < TSN_MAX_FLOWS; i++) {
    if (!g_flow_table[i].active) {
      g_flow_table[i].id = *id;
      g_flow_table[i].entry = *props;
      g_flow_table[i].active = true;
      printf("[TSN Flow] Added Flow %u (Class %d)\n", props->flow_id,
             props->traffic_class);
      return 0;
    }
  }
  return -1; // Table full
}

int tsn_flow_remove(uint32_t flow_id) {
  for (int i = 0; i < TSN_MAX_FLOWS; i++) {
    if (g_flow_table[i].active && g_flow_table[i].entry.flow_id == flow_id) {
      g_flow_table[i].active = false;
      printf("[TSN Flow] Removed Flow %u\n", flow_id);
      return 0;
    }
  }
  return -1; // Not found
}

tsn_flow_entry_t *tsn_flow_get(uint32_t flow_id) {
  for (int i = 0; i < TSN_MAX_FLOWS; i++) {
    if (g_flow_table[i].active && g_flow_table[i].entry.flow_id == flow_id) {
      return &g_flow_table[i].entry;
    }
  }
  return NULL;
}

uint32_t tsn_flow_classify(const tsn_flow_id_t *packet_header) {
  // 5-tuple match + VLAN Priority
  // Simplified: Exact match
  for (int i = 0; i < TSN_MAX_FLOWS; i++) {
    if (!g_flow_table[i].active)
      continue;

    tsn_flow_id_t *rule = &g_flow_table[i].id;

    if (rule->src_ip == packet_header->src_ip &&
        rule->dst_ip == packet_header->dst_ip &&
        rule->src_port == packet_header->src_port &&
        rule->dst_port == packet_header->dst_port &&
        rule->protocol == packet_header->protocol) {
      return g_flow_table[i].entry.flow_id;
    }
  }
  return 0; // Default/No Match
}
