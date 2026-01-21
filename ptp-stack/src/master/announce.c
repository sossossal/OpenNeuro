#include <string.h>
#include "announce.h"

/**
 * Initialize ANNOUNCE message
 */
int ptp_announce_init(ptp_announce_msg_t *msg)
{
    if (!msg) {
        return -1;
    }

    memset(msg, 0, sizeof(ptp_announce_msg_t));
    msg->steps_removed = 0;
    msg->time_source = 0xA0;  /* ATOMIC_CLOCK */
    
    return 0;
}

/**
 * Pack ANNOUNCE message for transmission
 */
int ptp_announce_pack(const ptp_announce_msg_t *msg, 
                      uint8_t *buffer, 
                      int buf_size)
{
    if (!msg || !buffer || buf_size < 64) {
        return -1;
    }

    int offset = 0;
    
    /* Copy payload data */
    memcpy(&buffer[offset], &msg->origin_timestamp, 10);
    offset += 10;
    
    memcpy(&buffer[offset], &msg->current_utc_offset, 2);
    offset += 2;
    
    buffer[offset++] = msg->priority1;
    buffer[offset++] = msg->clock_class;
    buffer[offset++] = msg->clock_accuracy;
    
    memcpy(&buffer[offset], &msg->offset_scaled_log_variance, 2);
    offset += 2;
    
    buffer[offset++] = msg->priority2;
    memcpy(&buffer[offset], &msg->grandmaster_identity, 8);
    offset += 8;
    
    memcpy(&buffer[offset], &msg->steps_removed, 2);
    offset += 2;
    
    buffer[offset++] = msg->time_source;
    
    return offset;
}

/**
 * Unpack ANNOUNCE message from buffer
 */
int ptp_announce_unpack(ptp_announce_msg_t *msg,
                        const uint8_t *buffer,
                        int buf_size)
{
    if (!msg || !buffer || buf_size < 64) {
        return -1;
    }

    int offset = 0;
    
    memcpy(&msg->origin_timestamp, &buffer[offset], 10);
    offset += 10;
    
    memcpy(&msg->current_utc_offset, &buffer[offset], 2);
    offset += 2;
    
    msg->priority1 = buffer[offset++];
    msg->clock_class = buffer[offset++];
    msg->clock_accuracy = buffer[offset++];
    
    memcpy(&msg->offset_scaled_log_variance, &buffer[offset], 2);
    offset += 2;
    
    msg->priority2 = buffer[offset++];
    memcpy(&msg->grandmaster_identity, &buffer[offset], 8);
    offset += 8;
    
    memcpy(&msg->steps_removed, &buffer[offset], 2);
    offset += 2;
    
    msg->time_source = buffer[offset++];
    
    return offset;
}
