#include "../../src/core/scheduler.h"
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#ifdef _MSC_VER
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

// Mock Queue Implementation for Benchmark
// (We link against actual queue.c/scheduler.c)

static inline uint64_t rdtsc() {
#ifdef _MSC_VER
  return __rdtsc();
#else
  unsigned int lo, hi;
  __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
  return ((uint64_t)hi << 32) | lo;
#endif
}

int main() {
  printf("=== TSN Scheduler Benchmark ===\n");

  tsn_scheduler_t *sched = tsn_scheduler_create();
  if (!sched)
    return 1;

  // Fill some queues
  // (In a real benchmark we would populate queues)

  uint64_t start, end;
  const int ITERATIONS = 100000;
  uint64_t current_time = 0;

  start = rdtsc();
  for (int i = 0; i < ITERATIONS; i++) {
    // Increment time to hit different slots
    current_time += 500; // 0.5us steps
    volatile tsn_frame_t *f =
        tsn_scheduler_select_next_frame(sched, current_time);
    (void)f;
  }
  end = rdtsc();

  double avg_cycles = (double)(end - start) / ITERATIONS;
  printf("Avg Cycles per Schedule Decision: %.2f\n", avg_cycles);
  printf("Est. Latency @ 2GHz: %.4f us\n", avg_cycles / 2000.0);

  tsn_scheduler_destroy(sched);
  return 0;
}
