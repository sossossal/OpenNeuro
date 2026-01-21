#include "clock.h"
#include <stdio.h>
#include <stdlib.h>


// Global Local Time (Software maintained)
static ptp_time_t g_internal_time = {0, 0};

// Mock Hardware Counter (In real system, this reads register)
static uint64_t mock_hw_counter_ns = 0;

void ptp_clock_init(void) {
  g_internal_time.seconds = 0;
  g_internal_time.nanoseconds = 0;
  mock_hw_counter_ns = 0;
}

void ptp_clock_normalize(ptp_time_t *time) {
  if (time->nanoseconds >= 1000000000) {
    time->seconds += time->nanoseconds / 1000000000;
    time->nanoseconds %= 1000000000;
  }
}

ptp_time_t ptp_clock_get_time(void) {
  // In real implementation:
  // return base_time + (hw_counter - last_counter_read)

  // Simulating time passage for now
  mock_hw_counter_ns += 1000; // Increment 1us per call

  ptp_time_t now = g_internal_time;
  now.nanoseconds += (uint32_t)(mock_hw_counter_ns % 1000000000);
  now.seconds += mock_hw_counter_ns / 1000000000;

  ptp_clock_normalize(&now);
  return now;
}

void ptp_clock_set_time(const ptp_time_t *timestamp) {
  g_internal_time = *timestamp;
  mock_hw_counter_ns = 0; // Reset HW counter conceptual base
}

void ptp_clock_adjtime(int64_t offset_ns) {
  // Simple Step implementation for now (Real PTP uses PI Controller to adjust
  // freq) If offset is small, we should slew. If large, step.

  int64_t total_ns = (int64_t)g_internal_time.nanoseconds + offset_ns;

  if (total_ns < 0) {
    g_internal_time.seconds -= 1;
    g_internal_time.nanoseconds = (uint32_t)(1000000000 + total_ns);
  } else {
    g_internal_time.nanoseconds = (uint32_t)total_ns;
  }
  ptp_clock_normalize(&g_internal_time);
}

bool ptp_clock_check_jump(const ptp_time_t *external_time) {
  ptp_time_t local = ptp_clock_get_time();

  int64_t diff_sec = (int64_t)external_time->seconds - (int64_t)local.seconds;

  // Check coarse threshold (1 second)
  if (abs(diff_sec) >= 1) {
    return true;
  }
  return false;
}
