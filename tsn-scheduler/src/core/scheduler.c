#include "scheduler.h"
#include <stdio.h>
#include <stdlib.h>

tsn_scheduler_t *tsn_scheduler_create(void) {
  tsn_scheduler_t *sched = (tsn_scheduler_t *)malloc(sizeof(tsn_scheduler_t));
  if (!sched)
    return NULL;

  // Initialize queues
  for (int i = 0; i < 8; i++) {
    sched->queues[i] = tsn_queue_create(1024); // Default size
  }

  // Default Cycle: 0-100us RT, 100-300us ST, 300-900us CT, 900-1000us BE
  sched->cycle_config.rt_slot_start = 0;
  sched->cycle_config.rt_slot_end = 100;
  sched->cycle_config.st_slot_start = 100;
  sched->cycle_config.st_slot_end = 300;
  sched->cycle_config.ct_slot_start = 300;
  sched->cycle_config.ct_slot_end = 900;
  sched->cycle_config.be_slot_start = 900;
  sched->cycle_config.be_slot_end = 1000;

  sched->gcl = NULL; // Not used in simplified mode yet

  return sched;
}

void tsn_scheduler_destroy(tsn_scheduler_t *sched) {
  if (sched) {
    for (int i = 0; i < 8; i++) {
      tsn_queue_destroy(sched->queues[i]);
    }
    free(sched);
  }
}

// Map Priority (0-7) to Traffic Class
static inline tsn_traffic_class_t get_traffic_class(uint8_t priority) {
  if (priority >= 6)
    return TSN_CLASS_RT; // 7, 6
  if (priority >= 4)
    return TSN_CLASS_ST; // 5, 4
  if (priority >= 2)
    return TSN_CLASS_CT; // 3, 2
  return TSN_CLASS_BE;   // 1, 0
}

bool tsn_can_transmit_packet(tsn_scheduler_t *sched, const tsn_frame_t *frame) {
  if (!sched || !frame)
    return false;

  // Mock Time: In real implementation, this comes from PTP
  // We assume current time is passed via context or queried here
  // For now, let's assume we need to pass time to this function,
  // or we update the declaration in header.
  // The previous header declaration had: tsn_scheduler_select_next_frame(...,
  // current_time_ns) But this function `tsn_can_transmit_packet` might need
  // time too. We will assume a global or struct-cached time for now to match
  // the signature in header, OR ideally we assume this is called *inside* the
  // select loop which knows time.

  // Warning: Header signature `tsn_can_transmit_packet` didn't take time.
  // Let's rely on `tsn_scheduler_select_next_frame` which takes time to perform
  // the check.
  return true;
}

tsn_frame_t *tsn_scheduler_select_next_frame(tsn_scheduler_t *sched,
                                             uint64_t current_time_ns) {
  if (!sched)
    return NULL;

  // 1. Calculate Cycle Offset (1ms cycle)
  uint32_t cycle_offset_us = (current_time_ns / 1000) % TSN_CYCLE_PERIOD_US;

  // 2. Pre-calculate Gate Mask (Optimize: avoid switch-case inside loop)
  uint8_t gate_mask = 0;
  // RT (7,6)
  if (cycle_offset_us >= sched->cycle_config.rt_slot_start &&
      cycle_offset_us < sched->cycle_config.rt_slot_end) {
    gate_mask |= 0xC0;
  }
  // ST (5,4)
  if (cycle_offset_us >= sched->cycle_config.st_slot_start &&
      cycle_offset_us < sched->cycle_config.st_slot_end) {
    gate_mask |= 0x30;
  }
  // CT (3,2)
  if (cycle_offset_us >= sched->cycle_config.ct_slot_start &&
      cycle_offset_us < sched->cycle_config.ct_slot_end) {
    gate_mask |= 0x0C;
  }
  // BE (1,0)
  if (cycle_offset_us >= sched->cycle_config.be_slot_start &&
      cycle_offset_us < sched->cycle_config.be_slot_end) {
    gate_mask |= 0x03;
  }

  // 3. Iterate Queues from High (7) to Low (0)
  // Optimization: Skip closed gates immediately
  for (int prio = 7; prio >= 0; prio--) {
    // Check gate bit
    if (!(gate_mask & (1 << prio))) {
      continue;
    }

    tsn_queue_t *q = sched->queues[prio];
    if (tsn_queue_is_empty(q))
      continue;

    // Peak head packet
    tsn_frame_t *frame = tsn_queue_peek(q);
    if (!frame)
      continue;

    // Gate is open (checked by mask), Transmit!
    return tsn_queue_dequeue(q);
  }

  return NULL; // No packet can be sent right now
}
