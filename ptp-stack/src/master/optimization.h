#ifndef PTP_OPTIMIZATION_H
#define PTP_OPTIMIZATION_H

#include "messages.h"
#include <stddef.h>
#include <stdint.h>


// Zero-Copy Buffer Handle
typedef struct {
  uint8_t *data;
  size_t len;
  void *hw_desc; // Hardware descriptor (DMA)
} ptp_zc_buffer_t;

// API
int ptp_opt_init_buffers(void);

// Get a zero-copy buffer for transmission
ptp_zc_buffer_t *ptp_opt_get_tx_buffer(void);

// Fast-path timestamp correction (One-Step Clock simulation)
// Modifies the packet *in-place* before TX if hardware supports it,
// or prepares field for Two-Step.
void ptp_opt_fast_timestamp_prep(ptp_zc_buffer_t *buf, uint64_t current_ts);

// Release buffer
void ptp_opt_release_buffer(ptp_zc_buffer_t *buf);

#endif // PTP_OPTIMIZATION_H
