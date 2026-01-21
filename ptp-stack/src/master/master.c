#include "master.h"
#include <stdio.h>
#include <string.h>

// Mock: Generate a Port ID based on MAC (stub)
static void get_default_port_id(uint8_t *id) {
  // 00:11:22:FF:FE:33:44:55 (EUI-64 from MAC)
  uint8_t mac_stub[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
  id[0] = mac_stub[0];
  id[1] = mac_stub[1];
  id[2] = mac_stub[2];
  id[3] = 0xFF;
  id[4] = 0xFE;
  id[5] = mac_stub[3];
  id[6] = mac_stub[4];
  id[7] = mac_stub[5];
  id[8] = 0x00;
  id[9] = 0x01; // Port Number
}

int ptp_master_init(ptp_master_ctx_t *ctx, const ptp_master_config_t *config) {
  if (!ctx || !config)
    return -1;

  memset(ctx, 0, sizeof(ptp_master_ctx_t));
  ctx->config = *config;

  get_default_port_id(ctx->port_identity);
  ctx->sequence_id = 0;

  ptp_clock_init();

  ctx->state = 1; // INITIALIZING
  ctx->is_running = true;

  printf("[PTP Master] Initialized on domain %d\n", config->domain_number);
  return 0;
}

void ptp_master_tick(ptp_master_ctx_t *ctx) {
  if (!ctx || !ctx->is_running)
    return;

  // Simple State Machine
  if (ctx->state == 1) {
    // Initializing -> Listening/Master
    // For simple Master-Only node, go straight to Master
    printf("[PTP Master] Transition to MASTER state\n");
    ctx->state = 3; // MASTER
  }

  if (ctx->state == 3) {
    // 1. Send Announce (1Hz)
    static int tick_cnt = 0;
    tick_cnt++;

    if (tick_cnt % 100 == 0) { // Assuming 10ms tick -> 100 ticks = 1s
      ptp_announce_message_t announce;
      ptp_msg_build_announce(&announce, ctx->port_identity, ctx->sequence_id++);
      // ptp_net_send(&announce, sizeof(announce)); // Network send stub
      ctx->announce_count++;
      // printf("[PTP Master] Sent Announce Seq=%d\n", ctx->sequence_id);
    }

    // 2. Send Sync (e.g. 0.125s -> 8Hz)
    if (tick_cnt % 12 == 0) { // Approx 8Hz
      ptp_sync_message_t sync;
      ptp_time_t now = ptp_clock_get_time();
      // In reality, we get TX timestamp AFTER send

      uint64_t ts_raw = (uint64_t)now.seconds * 1000000000 + now.nanoseconds;
      ptp_msg_build_sync(&sync, ctx->port_identity, ctx->sequence_id++, ts_raw);

      // ptp_net_send(&sync);

      // Follow Up
      ptp_follow_up_message_t followup;
      ptp_msg_build_follow_up(&followup, ctx->port_identity,
                              sync.header.sequence_id, ts_raw);
      // ptp_net_send(&followup);

      ctx->sync_count++;
    }
  }
}

void ptp_master_handle_packet(ptp_master_ctx_t *ctx, uint8_t *data,
                              size_t len) {
  // Parse header, check type, pass to bmc if Announce
  (void)ctx;
  (void)data;
  (void)len;
  // Stub
}
