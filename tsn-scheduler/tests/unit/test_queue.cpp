#include <gtest/gtest.h>
extern "C" {
#include "core/queue.h"
}

class TSNQueueTest : public ::testing::Test {
protected:
  void SetUp() override { queue = tsn_queue_create(5); }

  void TearDown() override { tsn_queue_destroy(queue); }

  tsn_queue_t *queue;
};

TEST_F(TSNQueueTest, CreateDestroy) {
  EXPECT_TRUE(queue != nullptr);
  EXPECT_EQ(tsn_queue_size(queue), 0);
  EXPECT_TRUE(tsn_queue_is_empty(queue));
}

TEST_F(TSNQueueTest, EnqueueDequeue) {
  tsn_frame_t frame1 = {0};
  frame1.flow_id = 1;

  tsn_frame_t frame2 = {0};
  frame2.flow_id = 2;

  EXPECT_TRUE(tsn_queue_enqueue(queue, &frame1));
  EXPECT_EQ(tsn_queue_size(queue), 1);
  EXPECT_FALSE(tsn_queue_is_empty(queue));

  EXPECT_TRUE(tsn_queue_enqueue(queue, &frame2));
  EXPECT_EQ(tsn_queue_size(queue), 2);

  tsn_frame_t *popped = tsn_queue_dequeue(queue);
  EXPECT_EQ(popped, &frame1);
  EXPECT_EQ(popped->flow_id, 1);
  EXPECT_EQ(tsn_queue_size(queue), 1);

  popped = tsn_queue_dequeue(queue);
  EXPECT_EQ(popped, &frame2);
  EXPECT_EQ(tsn_queue_size(queue), 0);
}

TEST_F(TSNQueueTest, FullQueue) {
  tsn_frame_t frames[5];
  for (int i = 0; i < 5; i++) {
    EXPECT_TRUE(tsn_queue_enqueue(queue, &frames[i]));
  }
  EXPECT_TRUE(tsn_queue_is_full(queue));

  tsn_frame_t extra;
  EXPECT_FALSE(tsn_queue_enqueue(queue, &extra));
}
