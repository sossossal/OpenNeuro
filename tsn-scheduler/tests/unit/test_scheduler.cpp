#include <gtest/gtest.h>
extern "C" {
#include "core/queue.h"
#include "core/scheduler.h"
}

class TSNSchedulerTest : public ::testing::Test {
protected:
  void SetUp() override { sched = tsn_scheduler_create(); }
  void TearDown() override { tsn_scheduler_destroy(sched); }
  tsn_scheduler_t *sched;
};

TEST_F(TSNSchedulerTest, SelectFrameInCorrectSlot) {
  // 1. Create RT Frame (Prio 7)
  tsn_frame_t rt_frame = {0};
  rt_frame.priority = 7; // RT Class
  tsn_queue_enqueue(sched->queues[7], &rt_frame);

  // 2. Check at t=50us (RT Window 0-100us) -> Should transmit
  tsn_frame_t *selected =
      tsn_scheduler_select_next_frame(sched, 50000); // 50us = 50000ns
  EXPECT_EQ(selected, &rt_frame);
}

TEST_F(TSNSchedulerTest, BlockFrameInWrongSlot) {
  // 1. Create RT Frame (Prio 7)
  tsn_frame_t rt_frame = {0};
  rt_frame.priority = 7; // RT Class
  tsn_queue_enqueue(sched->queues[7], &rt_frame);

  // 2. Check at t=150us (ST Window 100-300us) -> Should BLOCK RT
  // Note:Strict implementation might allow RT to preempt if guard band permits,
  // but basic Qbv blocks closed gates.
  tsn_frame_t *selected = tsn_scheduler_select_next_frame(sched, 150000);
  EXPECT_EQ(selected, nullptr);
}

TEST_F(TSNSchedulerTest, BestEffortSlot) {
  tsn_frame_t be_frame = {0};
  be_frame.priority = 0; // BE Class
  tsn_queue_enqueue(sched->queues[0], &be_frame);

  // t=950us (BE Window 900-1000us)
  tsn_frame_t *selected = tsn_scheduler_select_next_frame(sched, 950000);
  EXPECT_EQ(selected, &be_frame);
}
