#ifndef TAPRIO_IF_H
#define TAPRIO_IF_H

#include <stdbool.h>
#include <stdint.h>


// Mapping to Linux Kernel Structures (uapi/linux/pkt_sched.h)
// We replicate minimal needed structs for mapping our scheduler to TC commands

#define TC_TAPRIO_CMD_CONFIG 0x1

// A single entry in the GCL
struct tc_taprio_sched_entry {
  uint8_t command; // Set Gate CMD
  uint32_t gate_mask;
  uint32_t interval; // Nanoseconds
};

// Top level offload structure
struct tc_taprio_qopt_offload {
  uint8_t enable;
  uint32_t base_time_sec;
  uint32_t base_time_nsec;
  uint64_t cycle_time;
  uint64_t cycle_time_extension;
  size_t num_entries;
  struct tc_taprio_sched_entry entries[];
};

// API
// Initialize the Netlink socket for TC communication
int taprio_if_init(const char *interface_name);

// Apply the current OpenNeuro Scheduler config to Linux Kernel via Netlink
int taprio_if_apply_config(const struct tc_taprio_qopt_offload *config);

// Cleanup
void taprio_if_deinit(void);

#endif // TAPRIO_IF_H
