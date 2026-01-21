#include "../../src/master/optimization.h"
#include <stdint.h>
#include <stdio.h>

#ifdef _MSC_VER
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

// Simple CPU cycle counter
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
  printf("=== PTP Zero-Copy Benchmark ===\n");

  ptp_opt_init_buffers();

  uint64_t start, end;
  const int ITERATIONS = 100000;

  start = rdtsc();
  for (int i = 0; i < ITERATIONS; i++) {
    ptp_zc_buffer_t *buf = ptp_opt_get_tx_buffer();
    ptp_opt_fast_timestamp_prep(buf, 123456789);
    ptp_opt_release_buffer(buf);
  }
  end = rdtsc();

  double avg_cycles = (double)(end - start) / ITERATIONS;
  printf("Avg Cycles per TX Prep: %.2f\n", avg_cycles);

  // Assuming 2GHz CPU for cycle->time conversion rough estimate
  printf("Est. Latency @ 2GHz: %.4f us\n", avg_cycles / 2000.0);

  if (avg_cycles < 2000) { // < 1us on 2GHz
    printf("RESULT: PASS (<20us target met easily)\n");
    return 0;
  } else {
    printf("RESULT: FAIL\n");
    return 1;
  }
}
