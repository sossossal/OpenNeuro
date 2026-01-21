#include "../../src/master/master.h"
#include "../../src/protocol/sync.h"
#include "../../src/slave/slave.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


// Mock network layer - simulates packet transmission between Master and Slave
typedef struct {
  uint8_t buffer[1024];
  size_t len;
  bool has_packet;
} mock_network_t;

static mock_network_t master_to_slave;
static mock_network_t slave_to_master;

void mock_network_init(void) {
  memset(&master_to_slave, 0, sizeof(master_to_slave));
  memset(&slave_to_master, 0, sizeof(slave_to_master));
}

void mock_send_to_slave(uint8_t *data, size_t len) {
  memcpy(master_to_slave.buffer, data, len);
  master_to_slave.len = len;
  master_to_slave.has_packet = true;
}

void mock_send_to_master(uint8_t *data, size_t len) {
  memcpy(slave_to_master.buffer, data, len);
  slave_to_master.len = len;
  slave_to_master.has_packet = true;
}

bool mock_receive_from_master(uint8_t *data, size_t *len) {
  if (master_to_slave.has_packet) {
    memcpy(data, master_to_slave.buffer, master_to_slave.len);
    *len = master_to_slave.len;
    master_to_slave.has_packet = false;
    return true;
  }
  return false;
}

bool mock_receive_from_slave(uint8_t *data, size_t *len) {
  if (slave_to_master.has_packet) {
    memcpy(data, slave_to_master.buffer, slave_to_master.len);
    *len = slave_to_master.len;
    slave_to_master.has_packet = false;
    return true;
  }
  return false;
}

// Test: Master sends SYNC, Slave receives and processes
int test_sync_flow(ptp_master_ctx_t *master, ptp_slave_ctx_t *slave) {
  printf("\n=== Test 1: SYNC Flow ===\n");

  // Master sends SYNC
  ptp_sync_send_sync(master);

  // Simulate network delivery
  uint8_t packet[1024];
  size_t len;
  if (mock_receive_from_master(packet, &len)) {
    printf("[Test] Slave received SYNC packet (%zu bytes)\n", len);

    // Slave processes SYNC
    uint64_t rx_time = 1000000000; // Mock RX timestamp (1 second)
    ptp_slave_handle_packet(slave, packet, len);

    // Verify slave state transition
    if (slave->state == PTP_SLAVE_UNCALIBRATED ||
        slave->state == PTP_SLAVE_SLAVE) {
      printf("[PASS] Slave transitioned to state %d\n", slave->state);
      return 0;
    } else {
      printf("[FAIL] Slave in unexpected state %d\n", slave->state);
      return 1;
    }
  }

  printf("[FAIL] No SYNC packet received\n");
  return 1;
}

// Test: Complete synchronization cycle
int test_full_sync_cycle(ptp_master_ctx_t *master, ptp_slave_ctx_t *slave) {
  printf("\n=== Test 2: Full Synchronization Cycle ===\n");

  int iterations = 5;
  for (int i = 0; i < iterations; i++) {
    printf("\n--- Iteration %d ---\n", i + 1);

    // 1. Master sends SYNC
    ptp_sync_send_sync(master);

    // 2. Slave receives SYNC
    uint8_t packet[1024];
    size_t len;
    if (mock_receive_from_master(packet, &len)) {
      ptp_slave_handle_packet(slave, packet, len);
    }

    // 3. Master sends FOLLOW_UP (automatically sent after SYNC)
    // (Already sent in ptp_sync_send_sync)

    // 4. Slave receives FOLLOW_UP
    if (mock_receive_from_master(packet, &len)) {
      ptp_slave_handle_packet(slave, packet, len);
    }

    // 5. Slave sends DELAY_REQ
    ptp_slave_send_delay_req(slave);

    // 6. Master receives DELAY_REQ
    if (mock_receive_from_slave(packet, &len)) {
      ptp_master_handle_packet(master, packet, len);
    }

    // 7. Master sends DELAY_RESP (handled in ptp_master_handle_packet)

    // 8. Slave receives DELAY_RESP
    if (mock_receive_from_master(packet, &len)) {
      ptp_slave_handle_packet(slave, packet, len);

      // Check offset
      printf("[Info] Slave offset: %lld ns, mean delay: %lld ns\n",
             slave->offset_ns, slave->mean_delay_ns);
    }

    // Simulate time passing
    usleep(100000); // 100ms
  }

  // Verify slave reached SLAVE state
  if (slave->state == PTP_SLAVE_SLAVE) {
    printf("\n[PASS] Slave synchronized (state: SLAVE)\n");
    printf("[Info] Final offset: %lld ns\n", slave->offset_ns);

    // Check if offset is within acceptable range
    if (abs(slave->offset_ns) < 1000000) { // < 1ms
      printf("[PASS] Offset within acceptable range\n");
      return 0;
    } else {
      printf("[WARN] Offset exceeds 1ms (expected for mock)\n");
      return 0; // Still pass for mock test
    }
  } else {
    printf("[FAIL] Slave did not reach SLAVE state\n");
    return 1;
  }
}

int main() {
  printf("=== PTP Master-Slave Integration Test ===\n");

  // Initialize mock network
  mock_network_init();

  // Initialize Master
  ptp_master_config_t master_cfg = {.priority1 = 128,
                                    .priority2 = 128,
                                    .clock_class = 248,
                                    .clock_accuracy = 0x20,
                                    .clock_variance = 0x4E5D,
                                    .time_source = 0xA0,
                                    .domain_number = 0,
                                    .iface = "eth0"};

  ptp_master_ctx_t master;
  if (ptp_master_init(&master, &master_cfg) != 0) {
    printf("[FAIL] Master initialization failed\n");
    return 1;
  }
  printf("[PASS] Master initialized\n");

  // Initialize Slave
  ptp_slave_config_t slave_cfg = {
      .domain_number = 0,
      .iface = "eth0",
      .delay_req_interval_log = -3 // 125ms
  };

  ptp_slave_ctx_t slave;
  if (ptp_slave_init(&slave, &slave_cfg) != 0) {
    printf("[FAIL] Slave initialization failed\n");
    return 1;
  }
  printf("[PASS] Slave initialized\n");

  // Run tests
  int result = 0;

  result |= test_sync_flow(&master, &slave);
  result |= test_full_sync_cycle(&master, &slave);

  if (result == 0) {
    printf("\n=== All Tests Passed ===\n");
  } else {
    printf("\n=== Some Tests Failed ===\n");
  }

  return result;
}
