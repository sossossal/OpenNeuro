#ifndef PTP_TYPES_H
#define PTP_TYPES_H

#include <stdint.h>
#include <time.h>

/* PTP 时间戳类型 */
typedef struct {
    uint32_t seconds_high;
    uint32_t seconds_low;
    uint32_t nanoseconds;
} ptp_timestamp_t;

/* PTP 时钟信息 */
typedef struct {
    int64_t offset;              /* 纳秒 */
    int64_t frequency_offset;    /* ppb */
    int32_t delay;               /* 纳秒 */
    uint8_t state;               /* 时钟状态 */
} ptp_clock_info_t;

/* PTP 消息类型 */
typedef enum {
    PTP_MSG_SYNC = 0,
    PTP_MSG_DELAY_REQ = 1,
    PTP_MSG_PDELAY_REQ = 2,
    PTP_MSG_PDELAY_RESP = 3,
    PTP_MSG_FOLLOW_UP = 8,
    PTP_MSG_DELAY_RESP = 9,
    PTP_MSG_PDELAY_RESP_FOLLOW_UP = 10,
    PTP_MSG_ANNOUNCE = 11,
    PTP_MSG_SIGNALING = 12,
    PTP_MSG_MANAGEMENT = 13
} ptp_msg_type_t;

/* 端口状态 */
typedef enum {
    PTP_PORT_STATE_INIT = 1,
    PTP_PORT_STATE_FAULTY = 2,
    PTP_PORT_STATE_DISABLED = 3,
    PTP_PORT_STATE_LISTENING = 4,
    PTP_PORT_STATE_PRE_MASTER = 5,
    PTP_PORT_STATE_MASTER = 6,
    PTP_PORT_STATE_PASSIVE = 7,
    PTP_PORT_STATE_UNCALIBRATED = 8,
    PTP_PORT_STATE_SLAVE = 9
} ptp_port_state_t;

#endif /* PTP_TYPES_H */
