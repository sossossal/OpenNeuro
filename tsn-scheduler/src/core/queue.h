#ifndef TSN_QUEUE_H
#define TSN_QUEUE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Traffic Classes (Priority 0-7) mapped to 4 Classes
typedef enum {
  TSN_CLASS_RT = 0, // Real-Time: Priority 7-6 (<100us)
  TSN_CLASS_ST = 1, // Streaming: Priority 5-4 (<500us)
  TSN_CLASS_CT = 2, // Control: Priority 3-2 (<1ms)
  TSN_CLASS_BE = 3  // Best-Effort: Priority 1-0 (No guarantee)
} tsn_traffic_class_t;

// Frame/Packet Structure
typedef struct tsn_frame {
  uint8_t *data;
  uint16_t length;

  // Time Info
  uint64_t arrival_time_us;
  uint64_t deadline_us;

  // Classification
  uint8_t priority;      // 0-7
  uint8_t traffic_class; // RT/ST/CT/BE
  uint32_t flow_id;

  struct tsn_frame *next;
} tsn_frame_t;

// Queue Structure
typedef struct tsn_queue tsn_queue_t;

// API
tsn_queue_t *tsn_queue_create(size_t capacity);
void tsn_queue_destroy(tsn_queue_t *q);

bool tsn_queue_enqueue(tsn_queue_t *q, tsn_frame_t *frame);
tsn_frame_t *tsn_queue_dequeue(tsn_queue_t *q);
tsn_frame_t *tsn_queue_peek(tsn_queue_t *q);
bool tsn_queue_is_empty(tsn_queue_t *q);
bool tsn_queue_is_full(tsn_queue_t *q);
size_t tsn_queue_size(tsn_queue_t *q);

#endif // TSN_QUEUE_H
