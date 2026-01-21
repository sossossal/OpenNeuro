#include "optimization.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define NUM_ZC_BUFFERS 16
#define BUFFER_SIZE 256

// Pre-allocated static buffers to avoid malloc in critical path
static uint8_t raw_memory[NUM_ZC_BUFFERS][BUFFER_SIZE];
static ptp_zc_buffer_t buffer_pool[NUM_ZC_BUFFERS];
static int pool_free_idx = 0;

int ptp_opt_init_buffers(void) {
  for (int i = 0; i < NUM_ZC_BUFFERS; i++) {
    buffer_pool[i].data = raw_memory[i];
    buffer_pool[i].len = BUFFER_SIZE;
    buffer_pool[i].hw_desc = NULL;
  }
  pool_free_idx = 0;
  return 0;
}

ptp_zc_buffer_t *ptp_opt_get_tx_buffer(void) {
  // Simple ring allocation for demo
  // In real RTOS: use Lock-free Queue or Semaphore
  int idx = pool_free_idx;
  pool_free_idx = (pool_free_idx + 1) % NUM_ZC_BUFFERS;

  // Reset length
  buffer_pool[idx].len = 0;
  return &buffer_pool[idx];
}

void ptp_opt_fast_timestamp_prep(ptp_zc_buffer_t *buf, uint64_t current_ts) {
  // Optimization: Check command flag directly
  // If One-Step, insert timestamp now.
  // Here we simulate Two-Step preparation (store ID)

  (void)buf;
  (void)current_ts;
  // printf("[FastPath] Prepared buffer %p for TX at %llu\n", buf->data,
  // current_ts);
}

void ptp_opt_release_buffer(ptp_zc_buffer_t *buf) {
  (void)buf;
  // In ring buffer, we might not need explicit free if circular,
  // but for safety in simple impl we just leave it.
}
