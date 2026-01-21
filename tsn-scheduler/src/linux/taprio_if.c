#include "taprio_if.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Mocking Netlink for now, as we don't have a real Linux kernel environment in
// this simulation In real world: #include <linux/netlink.h>,
// <linux/rtnetlink.h>

static char g_interface[32];

int taprio_if_init(const char *interface_name) {
  if (!interface_name)
    return -1;
  strncpy(g_interface, interface_name, 31);
  printf("[TaprioIF] Initialized for interface: %s\n", g_interface);
  return 0;
}

int taprio_if_apply_config(const struct tc_taprio_qopt_offload *config) {
  if (!config)
    return -1;

  printf("[TaprioIF] Applying Config to %s:\n", g_interface);
  printf("  Base Time: %u.%u\n", config->base_time_sec, config->base_time_nsec);
  printf("  Cycle Time: %llu ns\n", config->cycle_time);
  printf("  Entries: %zu\n", config->num_entries);

  for (size_t i = 0; i < config->num_entries; i++) {
    printf("    [%zu] Mask: 0x%02x, Interval: %u ns\n", i,
           config->entries[i].gate_mask, config->entries[i].interval);
  }

  // In real implementation, this constructs a Netlink message (RTM_NEWQDISC)
  // and sends it to Kernel via libmnl or libnetlink.

  return 0; // Success
}

void taprio_if_deinit(void) { printf("[TaprioIF] Deinitialized.\n"); }
