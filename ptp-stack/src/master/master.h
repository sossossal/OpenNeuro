#ifndef PTP_MASTER_H
#define PTP_MASTER_H

#include "bmc.h"
#include "clock.h"
#include "messages.h"
#include <stdbool.h>
#include <stdint.h>


// Master Configuration
typedef struct {
  uint8_t priority1;
  uint8_t priority2;
  uint8_t clock_class;
  uint8_t clock_accuracy;
  uint16_t clock_variance;
  uint8_t time_source;
  uint8_t domain_number;
  const char *iface;
} ptp_master_config_t;

// Master Context
typedef struct {
  ptp_master_config_t config;
  uint8_t port_identity[10];
  uint16_t sequence_id;

  // Status
  bool is_running;
  uint8_t state; // PTP_M_INITIALIZING, etc.

  // Counters
  uint32_t announce_count;
  uint32_t sync_count;
} ptp_master_ctx_t;

// API
int ptp_master_init(ptp_master_ctx_t *ctx, const ptp_master_config_t *config);
void ptp_master_tick(ptp_master_ctx_t *ctx); // Called periodically (e.g. 10ms)
void ptp_master_handle_packet(ptp_master_ctx_t *ctx, uint8_t *data, size_t len);

#endif // PTP_MASTER_H
