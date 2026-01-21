#include "sync.h"
#include "../master/messages.h"
#include <stdio.h>


int ptp_sync_send_sync(ptp_master_ctx_t *ctx) {
  ptp_sync_message_t sync;

  // 1. Get current approximate time for Origin Timestamp
  ptp_time_t now = ptp_clock_get_time();
  uint64_t ts_raw = (uint64_t)now.seconds * 1000000000 + now.nanoseconds;

  // 2. Build SYNC
  ptp_msg_build_sync(&sync, ctx->port_identity, ctx->sequence_id, ts_raw);

  // 3. Send (Mock Network)
  // In real system: send_packet(sync) -> returns precise TX timestamp
  // Here we simulate the TX timestamp being slightly later than capture
  uint64_t precise_tx_ts = ts_raw + 500; // +500ns driver latency

  printf("[PTP SYNC] Sent Seq=%d, TS=%llu\n", ctx->sequence_id, precise_tx_ts);

  // 4. Send Follow_Up immediately (Two-Step Clock)
  ptp_sync_send_follow_up(ctx, ctx->sequence_id, precise_tx_ts);

  ctx->sequence_id++;
  return 0;
}

int ptp_sync_send_follow_up(ptp_master_ctx_t *ctx, uint16_t seq_id,
                            uint64_t precise_ts) {
  ptp_follow_up_message_t follow_up;

  // 1. Build Follow_Up with the PRECISE timestamp from step 3
  ptp_msg_build_follow_up(&follow_up, ctx->port_identity, seq_id, precise_ts);

  // 2. Send (Mock)
  printf("[PTP FollowUp] Sent Seq=%d, PreciseTS=%llu\n", seq_id, precise_ts);

  return 0;
}

void ptp_sync_process_interval(ptp_master_ctx_t *ctx) {
  if (ctx->state != 3)
    return; // Only Master sends Sync

  // In a real event loop, this is called when the Sync Interval timer expires
  // For simulation, we assume caller checks the timer
  ptp_sync_send_sync(ctx);
  ctx->sync_count++;
}
