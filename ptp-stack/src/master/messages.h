#ifndef PTP_MESSAGES_H
#define PTP_MESSAGES_H

#include <stdint.h>

// PTP Message Types
#define PTP_MSG_SYNC 0x0
#define PTP_MSG_DELAY_REQ 0x1
#define PTP_MSG_FOLLOW_UP 0x8
#define PTP_MSG_DELAY_RESP 0x9
#define PTP_MSG_ANNOUNCE 0xB
#define PTP_MSG_SIGNALING 0xC
#define PTP_MSG_MANAGEMENT 0xD

#if defined(_MSC_VER)
#define PTP_PACKED
#pragma pack(push, 1)
#else
#define PTP_PACKED __attribute__((packed))
#endif

// PTP Header (34 bytes)
typedef struct {
  uint8_t transport_specific : 4;
  uint8_t message_type : 4;
  uint8_t version_ptp : 4; // 0x02
  uint8_t reserved1 : 4;
  uint16_t message_length;
  uint8_t domain_number;
  uint8_t reserved2;
  uint16_t flag_field;
  int64_t correction_field;
  uint8_t reserved3[4];
  uint8_t source_port_identity[10];
  uint16_t sequence_id;
  uint8_t control_field;
  int8_t log_message_interval;
} PTP_PACKED ptp_header_t;

// ANNOUNCE Message (64 bytes total)
typedef struct {
  ptp_header_t header;
  struct {
    uint16_t origin_timestamp_seconds_msb;
    uint32_t origin_timestamp_seconds_lsb;
    uint32_t origin_timestamp_nanoseconds;
  } origin_timestamp;
  int16_t current_utc_offset;
  uint8_t reserved;
  uint8_t grandmaster_priority1;
  uint8_t grandmaster_clock_quality[4]; // Class, Accuracy, Variance
  uint8_t grandmaster_priority2;
  uint8_t grandmaster_identity[8];
  uint16_t steps_removed;
  uint8_t time_source;
} PTP_PACKED ptp_announce_message_t;

// SYNC Message (44 bytes total)
typedef struct {
  ptp_header_t header;
  struct {
    uint16_t seconds_msb;
    uint32_t seconds_lsb;
    uint32_t nanoseconds;
  } origin_timestamp;
} PTP_PACKED ptp_sync_message_t;

// FOLLOW_UP Message (44 bytes total)
typedef struct {
  ptp_header_t header;
  struct {
    uint16_t seconds_msb;
    uint32_t seconds_lsb;
    uint32_t nanoseconds;
  } precise_origin_timestamp;
} PTP_PACKED ptp_follow_up_message_t;

// DELAY_REQ Message
typedef struct {
  ptp_header_t header;
  struct {
    uint16_t seconds_msb;
    uint32_t seconds_lsb;
    uint32_t nanoseconds;
  } origin_timestamp;
} PTP_PACKED ptp_delay_req_message_t;

// DELAY_RESP Message (54 bytes total)
typedef struct {
  ptp_header_t header;
  struct {
    uint16_t seconds_msb;
    uint32_t seconds_lsb;
    uint32_t nanoseconds;
  } receive_timestamp;
  uint8_t requesting_port_identity[10];
} PTP_PACKED ptp_delay_resp_message_t;

#if defined(_MSC_VER)
#pragma pack(pop)
#endif

// Function Prototypes
void ptp_msg_build_header(ptp_header_t *header, uint8_t type, uint16_t len);
void ptp_msg_build_announce(ptp_announce_message_t *msg, const uint8_t *port_id,
                            uint16_t seq);
void ptp_msg_build_sync(ptp_sync_message_t *msg, const uint8_t *port_id,
                        uint16_t seq, uint64_t timestamp);
void ptp_msg_build_follow_up(ptp_follow_up_message_t *msg,
                             const uint8_t *port_id, uint16_t seq,
                             uint64_t precise_ts);

#endif // PTP_MESSAGES_H
