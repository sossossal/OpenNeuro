#ifndef PTP_SYNC_H
#define PTP_SYNC_H

#include "../master/master.h"

// API to handle Sync/FollowUp generation
void ptp_sync_process_interval(ptp_master_ctx_t *ctx);

// Handle specific message transmission (helper)
int ptp_sync_send_sync(ptp_master_ctx_t *ctx);
int ptp_sync_send_follow_up(ptp_master_ctx_t *ctx, uint16_t seq_id,
                            uint64_t precise_ts);

#endif // PTP_SYNC_H
