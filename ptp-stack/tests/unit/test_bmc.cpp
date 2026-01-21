#include <gtest/gtest.h>
extern "C" {
#include "master/bmc.h"
}

class PTPBMCTest : public ::testing::Test {};

TEST_F(PTPBMCTest, ComparePriority1) {
  ptp_dataset_t a = {0};
  a.priority1 = 100;
  ptp_dataset_t b = {0};
  b.priority1 = 200;

  // A is better (lower)
  EXPECT_EQ(ptp_bmc_compare(&a, &b), -1);

  // Swap
  EXPECT_EQ(ptp_bmc_compare(&b, &a), 1);
}

TEST_F(PTPBMCTest, CompareClass) {
  ptp_dataset_t a = {0};
  a.priority1 = 128;
  a.clock_class = 6;
  ptp_dataset_t b = {0};
  b.priority1 = 128;
  b.clock_class = 248;

  // A is better
  EXPECT_EQ(ptp_bmc_compare(&a, &b), -1);
}

TEST_F(PTPBMCTest, CompareIdentity) {
  ptp_dataset_t a = {0};
  a.priority1 = 128;
  a.clock_class = 248;
  a.clock_accuracy = 0xFE;
  a.clock_variance = 5000;
  a.priority2 = 128;
  memset(a.clock_identity, 0x01, 8); // ...010101

  ptp_dataset_t b = a;
  memset(b.clock_identity, 0x02, 8); // ...020202

  // A is better (lexicographically smaller)
  EXPECT_EQ(ptp_bmc_compare(&a, &b), -1);
}
