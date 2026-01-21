#include "queue.h"
#include <stdlib.h>
#include <string.h>

// Implementation using a Ring Buffer to store pointers to frames
// This avoids copying the frame data itself, just the pointers.

struct tsn_queue {
  tsn_frame_t **buffer; // Array of pointers
  size_t capacity;
  size_t size;
  size_t head;
  size_t tail;
  // pthread_mutex_t lock; // TODO: Add locking for MT safety
};

tsn_queue_t *tsn_queue_create(size_t capacity) {
  if (capacity == 0)
    return NULL;

  tsn_queue_t *q = (tsn_queue_t *)malloc(sizeof(tsn_queue_t));
  if (!q)
    return NULL;

  q->buffer = (tsn_frame_t **)calloc(capacity, sizeof(tsn_frame_t *));
  if (!q->buffer) {
    free(q);
    return NULL;
  }

  q->capacity = capacity;
  q->size = 0;
  q->head = 0;
  q->tail = 0;
  return q;
}

void tsn_queue_destroy(tsn_queue_t *q) {
  if (q) {
    if (q->buffer)
      free(q->buffer);
    free(q);
  }
}

bool tsn_queue_enqueue(tsn_queue_t *q, tsn_frame_t *frame) {
  if (!q || !frame)
    return false;

  // Lock
  if (q->size >= q->capacity) {
    // Unlock
    return false;
  }

  q->buffer[q->tail] = frame;
  q->tail = (q->tail + 1) % q->capacity;
  q->size++;

  // Unlock
  return true;
}

tsn_frame_t *tsn_queue_dequeue(tsn_queue_t *q) {
  if (!q || q->size == 0) {
    return NULL;
  }

  // Lock
  tsn_frame_t *frame = q->buffer[q->head];
  q->head = (q->head + 1) % q->capacity;
  q->size--;
  // Unlock

  return frame;
}

tsn_frame_t *tsn_queue_peek(tsn_queue_t *q) {
  if (!q || q->size == 0) {
    return NULL;
  }
  return q->buffer[q->head];
}

bool tsn_queue_is_empty(tsn_queue_t *q) {
  // Lock unnecessary for atomic read usually, but safer with lock
  return q ? (q->size == 0) : true;
}

bool tsn_queue_is_full(tsn_queue_t *q) {
  return q ? (q->size == q->capacity) : true;
}

size_t tsn_queue_size(tsn_queue_t *q) { return q ? q->size : 0; }
