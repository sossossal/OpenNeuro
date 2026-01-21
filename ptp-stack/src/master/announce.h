#ifndef PTP_ANNOUNCE_H
#define PTP_ANNOUNCE_H

#include <stdint.h>
#include "../common/types.h"

/* ANNOUNCE 消息结构 */
typedef struct {
    ptp_timestamp_t origin_timestamp;
    int16_t current_utc_offset;
    uint8_t priority1;
    uint8_t clock_class;
    uint8_t clock_accuracy;
    uint16_t offset_scaled_log_variance;
    uint8_t priority2;
    uint8_t grandmaster_identity[8];
    uint16_t steps_removed;
    uint8_t time_source;
} ptp_announce_msg_t;

/* API */
int ptp_announce_init(ptp_announce_msg_t *msg);
int ptp_announce_pack(const ptp_announce_msg_t *msg, uint8_t *buffer, int buf_size);
int ptp_announce_unpack(ptp_announce_msg_t *msg, const uint8_t *buffer, int buf_size);

#endif /* PTP_ANNOUNCE_H */
