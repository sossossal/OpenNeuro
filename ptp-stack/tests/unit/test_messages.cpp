#include <gtest/gtest.h>
extern "C" {
#include "master/messages.h"
}

// Helper to check big endian packing
uint16_t read_u16_be(const void *p) {
  const uint8_t *b = (const uint8_t *)p;
  return (b[0] << 8) | b[1];
}

class PTPMessagesTest : public ::testing::Test {
protected:
  uint8_t port_id[10] = {0x00, 0x11, 0x22, 0x33, 0x44,
                         0x55, 0x66, 0x77, 0x88, 0x99};
};

TEST_F(PTPMessagesTest, BuildAnnounce) {
  ptp_announce_message_t msg;
  ptp_msg_build_announce(&msg, port_id, 100);

  EXPECT_EQ(msg.header.message_type, 0xB); // ANNOUNCE
  EXPECT_EQ(read_u16_be(&msg.header.message_length),
            sizeof(ptp_announce_message_t));
  EXPECT_EQ(read_u16_be(&msg.header.sequence_id), 100);
  EXPECT_EQ(memcmp(msg.header.source_port_identity, port_id, 10), 0);

  EXPECT_EQ(msg.grandmaster_priority1, 128);
}

TEST_F(PTPMessagesTest, BuildSync) {
  ptp_sync_message_t msg;
  uint64_t ts = 123456789;
  ptp_msg_build_sync(&msg, port_id, 200, ts);

  EXPECT_EQ(msg.header.message_type, 0x0); // SYNC
  EXPECT_EQ(read_u16_be(&msg.header.sequence_id), 200);
  EXPECT_EQ(msg.header.control_field, 0x00);
}
