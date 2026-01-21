#include "bmc.h"

int ptp_bmc_compare(const ptp_dataset_t *a, const ptp_dataset_t *b) {
  // 1. Priority 1 (Lower is better)
  if (a->priority1 < b->priority1)
    return -1;
  if (a->priority1 > b->priority1)
    return 1;

  // 2. Clock Class (Lower is better)
  if (a->clock_class < b->clock_class)
    return -1;
  if (a->clock_class > b->clock_class)
    return 1;

  // 3. Clock Accuracy (Lower is better)
  if (a->clock_accuracy < b->clock_accuracy)
    return -1;
  if (a->clock_accuracy > b->clock_accuracy)
    return 1;

  // 4. Clock Variance (Lower is better)
  if (a->clock_variance < b->clock_variance)
    return -1;
  if (a->clock_variance > b->clock_variance)
    return 1;

  // 5. Priority 2 (Lower is better)
  if (a->priority2 < b->priority2)
    return -1;
  if (a->priority2 > b->priority2)
    return 1;

  // 6. Clock Identity (Lower lexical value is better)
  int cmp = memcmp(a->clock_identity, b->clock_identity, 8);
  if (cmp < 0)
    return -1;
  if (cmp > 0)
    return 1;

  // Steps Removed handling usually happens before this or as a tie-breaker
  // depending on context Ideally, "shorter steps" is preferred in some
  // topologies, but standard BMC mainly focuses on Clock Quality first. We will
  // stick to standard datasets comparison here.

  // 7. Port Number (system specific tie breaker)
  if (a->port_number < b->port_number)
    return -1;
  if (a->port_number > b->port_number)
    return 1;

  return 0;
}
