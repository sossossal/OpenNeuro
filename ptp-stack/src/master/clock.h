#ifndef PTP_CLOCK_H
#define PTP_CLOCK_H

#include <stdbool.h>
#include <stdint.h>


// PTP Time Structure (96-bit internal representation usually, simplified here
// to 64s+32ns)
typedef struct {
  uint64_t seconds;
  uint32_t nanoseconds;
} ptp_time_t;

// API
void ptp_clock_init(void);

// Get current PTP time (Software estimate + Hardware counter)
ptp_time_t ptp_clock_get_time(void);

// Set PTP time (Step time)
void ptp_clock_set_time(const ptp_time_t *timestamp);

// Adjust time (slew) - Placeholder for servo loop
void ptp_clock_adjtime(int64_t offset_ns);

// Check if external master time differs significantly from local time
// Returns true if jump > threshold (e.g. 1s)
bool ptp_clock_check_jump(const ptp_time_t *external_time);

// Helper: Normalize time (handle ns overflow)
void ptp_clock_normalize(ptp_time_t *time);

#endif // PTP_CLOCK_H
