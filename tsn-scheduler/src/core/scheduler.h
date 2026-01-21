#ifndef TSN_SCHEDULER_H
#define TSN_SCHEDULER_H

#include "queue.h"

// Cycle Period
#define TSN_CYCLE_PERIOD_US 1000 // 1ms

// GCL (Gate Control List) Entry
typedef struct {
  uint8_t gate_state; // Bitmask for 8 queues (1 = Open, 0 = Closed)
  uint32_t time_interval_ns;
} tsn_gcl_entry_t;

// Cycle Configuration
typedef struct {
  uint32_t rt_slot_start;
  uint32_t rt_slot_end;

  uint32_t st_slot_start;
  uint32_t st_slot_end;

  uint32_t ct_slot_start;
  uint32_t ct_slot_end;

  uint32_t be_slot_start;
  uint32_t be_slot_end;
} tsn_cycle_config_t;

// Scheduler Structure
typedef struct {
  tsn_queue_t *queues[8]; // 8 Traffic Classes (Priority 7-0)
  void *gcl;              // Abstracted (Placeholder for full GCL struct)
  tsn_cycle_config_t cycle_config;
} tsn_scheduler_t;

// API
tsn_scheduler_t *tsn_scheduler_create(void);
void tsn_scheduler_destroy(tsn_scheduler_t *sched);
bool tsn_can_transmit_packet(tsn_scheduler_t *sched, const tsn_frame_t *frame);
tsn_frame_t *tsn_scheduler_select_next_frame(tsn_scheduler_t *sched,
                                             uint64_t current_time_ns);

#endif // TSN_SCHEDULER_H
