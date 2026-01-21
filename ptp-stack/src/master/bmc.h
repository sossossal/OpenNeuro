#ifndef PTP_BMC_H
#define PTP_BMC_H

#include <stdint.h>
#include <string.h>

// Dataset for BMC comparison
typedef struct {
  uint8_t priority1;
  uint8_t clock_class;
  uint8_t clock_accuracy;
  uint16_t clock_variance;
  uint8_t priority2;
  uint8_t clock_identity[8];
  uint16_t steps_removed;
  uint16_t port_number; // Tie breaker
} ptp_dataset_t;

// API
/**
 * Compare two datasets A and B.
 * Returns:
 *  -1 if A is better than B
 *   1 if B is better than A
 *   0 if they are equal
 */
int ptp_bmc_compare(const ptp_dataset_t *a, const ptp_dataset_t *b);

#endif // PTP_BMC_H
