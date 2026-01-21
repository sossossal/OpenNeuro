#include "messages.h"
#include "../common/ptp_common.h"
#include <string.h>

#define PTP_VERSION 0x02

void ptp_msg_build_header(ptp_header_t *header, uint8_t type, uint16_t len) {
  memset(header, 0, sizeof(ptp_header_t));

  // Transport specific (0) + Message Type
  header->transport_specific = 0x0;
  header->message_type = type;

  // Version PTP (2) + Reserved
  header->version_ptp = PTP_VERSION;
  header->reserved1 = 0;

  // Length in Big Endian
  header->message_length = HTONS(len);

  header->domain_number = 0;
  header->reserved2 = 0;
  header->flag_field = 0; // Flags usually set per message type
  header->correction_field = 0;

  // Source Port Identity (10 bytes) - Caller sets this usually
  // memset(header->source_port_identity, 0, 10);

  header->sequence_id = 0;   // Caller sets this
  header->control_field = 0; // Legacy, filled by specific type if needed
  header->log_message_interval = 0x7F; // Default
}

void ptp_msg_build_announce(ptp_announce_message_t *msg, const uint8_t *port_id,
                            uint16_t seq) {
  memset(msg, 0, sizeof(ptp_announce_message_t));

  ptp_msg_build_header(&msg->header, PTP_MSG_ANNOUNCE,
                       sizeof(ptp_announce_message_t));

  if (port_id)
    memcpy(msg->header.source_port_identity, port_id, 10);
  msg->header.sequence_id = HTONS(seq);
  msg->header.log_message_interval = 1; // 2^1 = 2s log interval
  msg->header.control_field = 0x05;     // Control field for Announce (legacy)

  // Payload (Big Endian)
  msg->current_utc_offset = HTONS(37);
  msg->grandmaster_priority1 = 128;

  // Grandmaster Clock Quality
  msg->grandmaster_clock_quality[0] = 6;    // Class
  msg->grandmaster_clock_quality[1] = 0x20; // Accuracy (example)
  msg->grandmaster_clock_quality[2] = 0;    // Variance MSB
  msg->grandmaster_clock_quality[3] = 0;    // Variance LSB

  msg->grandmaster_priority2 = 128;
  msg->steps_removed = HTONS(0);
  msg->time_source = 0xA0; // Internal Oscillator
}

void ptp_msg_build_sync(ptp_sync_message_t *msg, const uint8_t *port_id,
                        uint16_t seq, uint64_t timestamp) {
  memset(msg, 0, sizeof(ptp_sync_message_t));

  ptp_msg_build_header(&msg->header, PTP_MSG_SYNC, sizeof(ptp_sync_message_t));

  if (port_id)
    memcpy(msg->header.source_port_identity, port_id, 10);
  msg->header.sequence_id = HTONS(seq);
  msg->header.control_field = 0x00;      // Sync
  msg->header.log_message_interval = -3; // 2^-3 = 0.125s (Example)
  /* Note: log_message_interval is int8_t, no endian swap needed */

  // Timestamp
  uint16_t sec_msb = (uint16_t)((timestamp >> 48) & 0xFFFF);
  uint32_t sec_lsb = (uint32_t)((timestamp >> 16) & 0xFFFFFFFF);
  uint32_t ns = (uint32_t)(timestamp & 0xFFFF); // Placeholder logic

  msg->origin_timestamp.seconds_msb = HTONS(sec_msb);
  msg->origin_timestamp.seconds_lsb = HTONL(sec_lsb);
  msg->origin_timestamp.nanoseconds = HTONL(ns);
}

void ptp_msg_build_follow_up(ptp_follow_up_message_t *msg,
                             const uint8_t *port_id, uint16_t seq,
                             uint64_t precise_ts) {
  memset(msg, 0, sizeof(ptp_follow_up_message_t));

  ptp_msg_build_header(&msg->header, PTP_MSG_FOLLOW_UP,
                       sizeof(ptp_follow_up_message_t));

  if (port_id)
    memcpy(msg->header.source_port_identity, port_id, 10);
  msg->header.sequence_id = HTONS(seq);
  msg->header.control_field = 0x02; // Follow_Up
  msg->header.log_message_interval = -3;

  // Precise Timestamp
  uint16_t sec_msb = (uint16_t)((precise_ts >> 48) & 0xFFFF);
  uint32_t sec_lsb = (uint32_t)((precise_ts >> 16) & 0xFFFFFFFF);
  uint32_t ns = (uint32_t)(precise_ts & 0xFFFF);

  msg->precise_origin_timestamp.seconds_msb = HTONS(sec_msb);
  msg->precise_origin_timestamp.seconds_lsb = HTONL(sec_lsb);
  msg->precise_origin_timestamp.nanoseconds = HTONL(ns);
}
