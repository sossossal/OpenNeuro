#include <gtest/gtest.h>
extern "C" {
#include "core/bandwidth.h"
}

TEST(TSNBandwidthTest, AdmissionControl) {
  tsn_bandwidth_init();

  // Class RT Limit is 400 Mbps
  uint32_t ok_flow = 100000000;   // 100 Mbps
  uint32_t huge_flow = 500000000; // 500 Mbps

  EXPECT_TRUE(tsn_bandwidth_check(TSN_CLASS_RT, ok_flow));
  EXPECT_FALSE(tsn_bandwidth_check(TSN_CLASS_RT, huge_flow));

  // Reserve
  tsn_bandwidth_reserve(TSN_CLASS_RT, ok_flow);
  EXPECT_EQ(tsn_bandwidth_get_usage(TSN_CLASS_RT), 25); // 100/400 = 25%

  // Release
  tsn_bandwidth_release(TSN_CLASS_RT, ok_flow);
  EXPECT_EQ(tsn_bandwidth_get_usage(TSN_CLASS_RT), 0);
}
