#include "slave.h"
#include "servo.h"
#include <stdio.h>
#include <string.h>

int ptp_slave_init(ptp_slave_ctx_t *ctx, const ptp_slave_config_t *config) {
  memset(ctx, 0, sizeof(ptp_slave_ctx_t));
  ctx->config = *config;
  ctx->state = PTP_SLAVE_INITIALIZING;
  ctx->sequence_id = 0;

  // Generate port identity (simplified - use MAC + port number)
  memset(ctx->port_identity, 0x02, 8); // Mock MAC
  ctx->port_identity[8] = 0;
  ctx->port_identity[9] = 1;

  ctx->state = PTP_SLAVE_LISTENING;
  ctx->is_running = true;

  printf("[PTP Slave] Initialized in LISTENING state\n");
  return 0;
}

void ptp_slave_tick(ptp_slave_ctx_t *ctx) {
  if (!ctx->is_running)
    return;

  // State machine
  switch (ctx->state) {
  case PTP_SLAVE_LISTENING:
    // Wait for SYNC from Master
    // Transition happens in ptp_slave_process_sync()
    break;

  case PTP_SLAVE_UNCALIBRATED:
  case PTP_SLAVE_SLAVE:
    // Send DELAY_REQ periodically
    if (ctx->delay_req_timer > 0) {
      ctx->delay_req_timer--;
    } else {
      ptp_slave_send_delay_req(ctx);
      // Reset timer (e.g., 125ms = 12.5 ticks if tick = 10ms)
      int interval_ms = 1000 >> (-ctx->config.delay_req_interval_log);
      ctx->delay_req_timer = interval_ms / 10; // Assuming 10ms tick
    }
    break;

  default:
    break;
  }
}

void ptp_slave_handle_packet(ptp_slave_ctx_t *ctx, uint8_t *data, size_t len) {
  if (len < sizeof(ptp_header_t))
    return;

  ptp_header_t *hdr = (ptp_header_t *)data;
  uint8_t msg_type = hdr->message_type;

  uint64_t rx_time = 0; // TODO: Get hardware timestamp

  switch (msg_type) {
  case PTP_MSG_SYNC:
    ptp_slave_process_sync(ctx, (ptp_sync_message_t *)data, rx_time);
    break;
  case PTP_MSG_FOLLOW_UP:
    ptp_slave_process_follow_up(ctx, (ptp_follow_up_message_t *)data);
    break;
  case PTP_MSG_DELAY_RESP:
    ptp_slave_process_delay_resp(ctx, (ptp_delay_resp_message_t *)data);
    break;
  default:
    break;
  }
}

void ptp_slave_process_sync(ptp_slave_ctx_t *ctx, const ptp_sync_message_t *msg,
                            uint64_t rx_time) {
  // Store t2 (RX timestamp)
  ctx->t2_ns = rx_time;

  // Transition to UNCALIBRATED if in LISTENING
  if (ctx->state == PTP_SLAVE_LISTENING) {
    memcpy(ctx->master_port_identity, msg->header.source_port_identity, 10);
    ctx->master_selected = true;
    ctx->state = PTP_SLAVE_UNCALIBRATED;
    printf("[PTP Slave] Master selected, state → UNCALIBRATED\n");
  }

  ctx->sync_count++;
}

void ptp_slave_process_follow_up(ptp_slave_ctx_t *ctx,
                                 const ptp_follow_up_message_t *msg) {
  // Extract t1 (Master TX timestamp from FOLLOW_UP)
  ctx->t1_ns =
      ((uint64_t)msg->precise_origin_timestamp.seconds_lsb * 1000000000ULL) +
      msg->precise_origin_timestamp.nanoseconds;

  printf("[PTP Slave] FOLLOW_UP received, t1=%llu ns\n", ctx->t1_ns);
}

void ptp_slave_process_delay_resp(ptp_slave_ctx_t *ctx,
                                  const ptp_delay_resp_message_t *msg) {
  // Extract t4 (Master RX timestamp from DELAY_RESP)
  ctx->t4_ns = ((uint64_t)msg->receive_timestamp.seconds_lsb * 1000000000ULL) +
               msg->receive_timestamp.nanoseconds;

  // Calculate offset and mean delay
  // offset = t2 - t1 - mean_delay
  // mean_delay = ((t2 - t1) + (t4 - t3)) / 2
  int64_t forward_delay = (int64_t)(ctx->t2_ns - ctx->t1_ns);
  int64_t reverse_delay = (int64_t)(ctx->t4_ns - ctx->t3_ns);

  ctx->mean_delay_ns = (forward_delay + reverse_delay) / 2;
  ctx->offset_ns = forward_delay - ctx->mean_delay_ns;

  printf("[PTP Slave] Offset: %lld ns, Mean Delay: %lld ns\n", ctx->offset_ns,
         ctx->mean_delay_ns);

  // Apply servo correction
  int64_t adjustment = ptp_servo_update(ctx->offset_ns, ctx->mean_delay_ns);
  printf("[PTP Slave] Servo adjustment: %lld ns\n", adjustment);

  // Transition to SLAVE if in UNCALIBRATED
  if (ctx->state == PTP_SLAVE_UNCALIBRATED) {
    ctx->state = PTP_SLAVE_SLAVE;
    printf("[PTP Slave] State → SLAVE (synchronized)\n");
  }
}

void ptp_slave_send_delay_req(ptp_slave_ctx_t *ctx) {
  ptp_delay_req_message_t req;

  // Build DELAY_REQ message
  ptp_msg_build_header(&req.header, PTP_MSG_DELAY_REQ, sizeof(req));
  memcpy(req.header.source_port_identity, ctx->port_identity, 10);
  req.header.sequence_id = ctx->sequence_id++;

  // Set origin timestamp (t3) - will be updated by hardware on TX
  ctx->t3_ns = 0; // TODO: Get actual TX timestamp

  // Send (mock)
  printf("[PTP Slave] Sent DELAY_REQ, seq=%d\n", req.header.sequence_id);

  ctx->delay_req_count++;
}
