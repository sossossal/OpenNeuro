#include "../linux/netlink_socket.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  const char *iface = "eth0";

  if (argc > 1) {
    iface = argv[1];
  }

  printf("=== Linux Taprio Integration Test ===\n");
  printf("Interface: %s\n\n", iface);

  // 1. 初始化 Netlink
  printf("1. Initializing Netlink socket...\n");
  nl_sock_ctx_t ctx;
  if (netlink_socket_init(&ctx) != 0) {
    printf("[FAIL] Netlink initialization failed\n");
    printf("Note: This requires Linux with CONFIG_NET_SCHED_TAPRIO\n\n");
    return 1;
  }
  printf("[PASS] Netlink initialized\n\n");

  // 2. 配置 Taprio
  printf("2. Configuring Taprio...\n");
  tsn_config_t config;
  memset(&config, 0, sizeof(config));

  config.num_tc = 4;
  config.base_time = 0;
  config.cycle_time = 1000000; // 1ms
  config.num_gates = 4;

  // 门控配置: 4 个时间槽，每个 250µs
  config.gates[0].gate_mask = 0xC0; // RT (优先级 7,6)
  config.gates[0].duration = 250000;

  config.gates[1].gate_mask = 0x30; // ST (优先级 5,4)
  config.gates[1].duration = 250000;

  config.gates[2].gate_mask = 0x0C; // CT (优先级 3,2)
  config.gates[2].duration = 250000;

  config.gates[3].gate_mask = 0x03; // BE (优先级 1,0)
  config.gates[3].duration = 250000;

  if (netlink_apply_taprio(&ctx, iface, &config) != 0) {
    printf("[FAIL] Taprio configuration failed\n");
    printf("Note: This requires:\n");
    printf("  - Root privileges (run with sudo)\n");
    printf("  - Linux kernel 5.15+ with CONFIG_NET_SCHED_TAPRIO\n");
    printf("  - Network interface %s exists\n\n", iface);
    netlink_socket_cleanup(&ctx);
    return 1;
  }
  printf("[PASS] Taprio configured\n\n");

  // 3. 查询状态
  printf("3. Querying Taprio status...\n");
  tsn_status_t status;
  if (netlink_get_taprio_status(&ctx, iface, &status) == 0) {
    printf("[PASS] Status retrieved\n");
    printf("  Interface: %s\n", status.ifname);
    printf("  Active: %s\n", status.active ? "Yes" : "No");
  } else {
    printf("[WARN] Status query failed\n");
  }
  printf("\n");

  // 4. 清理
  printf("4. Cleanup...\n");
  netlink_socket_cleanup(&ctx);
  printf("[PASS] Cleaned up\n\n");

  printf("=== Test Complete ===\n");
  printf("\nTo verify Taprio is active, run:\n");
  printf("  tc qdisc show dev %s\n", iface);
  printf("\nTo remove Taprio:\n");
  printf("  sudo tc qdisc del dev %s root\n", iface);

  return 0;
}
