#ifndef PTP_SLAVE_H
#define PTP_SLAVE_H

#include "../master/clock.h"
#include "../master/messages.h"
#include <stdbool.h>
#include <stdint.h>


// Slave States
typedef enum {
  PTP_SLAVE_INITIALIZING = 0,
  PTP_SLAVE_LISTENING = 1,
  PTP_SLAVE_UNCALIBRATED = 2,
  PTP_SLAVE_SLAVE = 3,
  PTP_SLAVE_FAULTY = 4
} ptp_slave_state_t;

// Slave Configuration
typedef struct {
  uint8_t domain_number;
  const char *iface;
  int delay_req_interval_log; // Log2 of interval in seconds (e.g., -3 = 125ms)
} ptp_slave_config_t;

// Slave Context
typedef struct {
  ptp_slave_config_t config;
  uint8_t port_identity[10];
  uint16_t sequence_id;

  // State
  ptp_slave_state_t state;
  bool is_running;

  // Master tracking
  uint8_t master_port_identity[10];
  bool master_selected;

  // Timestamps for offset calculation
  uint64_t t1_ns; // Master TX timestamp (from FOLLOW_UP)
  uint64_t t2_ns; // Slave RX timestamp (SYNC arrival)
  uint64_t t3_ns; // Slave TX timestamp (DELAY_REQ sent)
  uint64_t t4_ns; // Master RX timestamp (from DELAY_RESP)

  // Calculated values
  int64_t offset_ns;     // Clock offset from master
  int64_t mean_delay_ns; // Mean path delay

  // Counters
  uint32_t sync_count;
  uint32_t delay_req_count;
  uint32_t delay_req_timer; // Countdown for next DELAY_REQ
} ptp_slave_ctx_t;

// API
int ptp_slave_init(ptp_slave_ctx_t *ctx, const ptp_slave_config_t *config);
void ptp_slave_tick(ptp_slave_ctx_t *ctx); // Called periodically (e.g., 10ms)
void ptp_slave_handle_packet(ptp_slave_ctx_t *ctx, uint8_t *data, size_t len);

// Internal helpers (exposed for testing)
void ptp_slave_process_sync(ptp_slave_ctx_t *ctx, const ptp_sync_message_t *msg,
                            uint64_t rx_time);
void ptp_slave_process_follow_up(ptp_slave_ctx_t *ctx,
                                 const ptp_follow_up_message_t *msg);
void ptp_slave_process_delay_resp(ptp_slave_ctx_t *ctx,
                                  const ptp_delay_resp_message_t *msg);
void ptp_slave_send_delay_req(ptp_slave_ctx_t *ctx);

#endif // PTP_SLAVE_H
