#include <gtest/gtest.h>
extern "C" {
#include "core/flow.h"
}

TEST(TSNFlowTest, AddRemoveClassify) {
  tsn_flow_table_init();

  tsn_flow_id_t id = {0};
  id.src_ip = 0xC0A80001; // 192.168.0.1
  id.dst_ip = 0xC0A80002;
  id.src_port = 5000;
  id.dst_port = 6000;
  id.protocol = 17; // UDP

  tsn_flow_entry_t props = {0};
  props.flow_id = 99;
  props.traffic_class = TSN_CLASS_RT;

  // Add
  EXPECT_EQ(tsn_flow_add(&id, &props), 0);

  // Classify
  EXPECT_EQ(tsn_flow_classify(&id), 99);

  // Miss
  tsn_flow_id_t bad_id = id;
  bad_id.src_port = 9999;
  EXPECT_EQ(tsn_flow_classify(&bad_id), 0);

  // Remove
  EXPECT_EQ(tsn_flow_remove(99), 0);
  EXPECT_EQ(tsn_flow_classify(&id), 0);
}
