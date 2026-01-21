#include <gtest/gtest.h>
extern "C" {
#include "linux/taprio_config.h"
}

TEST(TSNTaprioConfigTest, GenerateConfig) {
  tsn_scheduler_t *sched = tsn_scheduler_create();

  // Convert to Taprio Config
  struct tc_taprio_qopt_offload *config = taprio_config_from_scheduler(sched);

  ASSERT_TRUE(config != nullptr);
  EXPECT_EQ(config->num_entries, 4);
  EXPECT_EQ(config->cycle_time, 1000000); // 1ms

  // Check RT Entry (Indices 0)
  // Mask 0xC0 (11000000) for Priority 7, 6
  EXPECT_EQ(config->entries[0].gate_mask, 0xC0);
  EXPECT_EQ(config->entries[0].interval, 100000); // 100us

  free(config);
  tsn_scheduler_destroy(sched);
}
