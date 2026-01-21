#include <gtest/gtest.h>
extern "C" {
#include "master/clock.h"
}

TEST(PTPClockTest, Normalize) {
  ptp_time_t t = {10, 1500000000}; // 10s + 1.5s
  ptp_clock_normalize(&t);
  EXPECT_EQ(t.seconds, 11);
  EXPECT_EQ(t.nanoseconds, 500000000);
}

TEST(PTPClockTest, CheckJump) {
  ptp_time_t t_now = {100, 0};
  ptp_clock_set_time(&t_now);

  ptp_time_t t_remote_ok = {100, 500};
  EXPECT_FALSE(ptp_clock_check_jump(&t_remote_ok));

  ptp_time_t t_remote_bad = {102, 0}; // 2s diff
  EXPECT_TRUE(ptp_clock_check_jump(&t_remote_bad));
}
