#include "taprio_config.h"
#include <stdlib.h>
#include <string.h>

struct tc_taprio_qopt_offload *
taprio_config_from_scheduler(const tsn_scheduler_t *sched) {
  if (!sched)
    return NULL;

  // 我们有 4 个窗口: RT, ST, CT, BE
  // 所以需要 4 个 GCL entries
  size_t num_entries = 4;
  size_t size = sizeof(struct tc_taprio_qopt_offload) +
                num_entries * sizeof(struct tc_taprio_sched_entry);

  struct tc_taprio_qopt_offload *opt =
      (struct tc_taprio_qopt_offload *)malloc(size);
  if (!opt)
    return NULL;

  memset(opt, 0, size);
  opt->enable = 1;
  opt->base_time_sec = 0; // Starts immediately (relative)
  opt->base_time_nsec = 0;
  opt->cycle_time = TSN_CYCLE_PERIOD_US * 1000; // 1ms = 1,000,000 ns
  opt->num_entries = num_entries;

  const tsn_cycle_config_t *cfg = &sched->cycle_config;

  // Entry 0: RT Window (Priority 7-6 Open)
  // Mask: bit 7 and 6 set -> 0xC0
  opt->entries[0].command = 0x00; // SetGate (CMD_SET_GATES)
  opt->entries[0].gate_mask = 0xC0;
  opt->entries[0].interval = (cfg->rt_slot_end - cfg->rt_slot_start) * 1000;

  // Entry 1: ST Window (Priority 5-4 Open)
  // Mask: bit 5 and 4 set -> 0x30
  opt->entries[1].command = 0x00;
  opt->entries[1].gate_mask = 0x30;
  opt->entries[1].interval = (cfg->st_slot_end - cfg->st_slot_start) * 1000;

  // Entry 2: CT Window (Priority 3-2 Open)
  // Mask: bit 3 and 2 set -> 0x0C
  opt->entries[2].command = 0x00;
  opt->entries[2].gate_mask = 0x0C;
  opt->entries[2].interval = (cfg->ct_slot_end - cfg->ct_slot_start) * 1000;

  // Entry 3: BE Window (Priority 1-0 Open)
  // Mask: bit 1 and 0 set -> 0x03
  opt->entries[3].command = 0x00;
  opt->entries[3].gate_mask = 0x03;
  opt->entries[3].interval = (cfg->be_slot_end - cfg->be_slot_start) * 1000;

  return opt;
}
