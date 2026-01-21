#include "../../ptp-stack/src/master/master.h"
#include "zenoh_hal.h"
#include <stdio.h>
#include <string.h>


// PTP Status message structure
typedef struct {
  int64_t offset_ns;
  uint32_t sync_count;
  uint8_t state;
} ptp_status_t;

static zenoh_publisher_t *g_ptp_pub = NULL;

int ptp_publisher_init(zenoh_session_t *session) {
  g_ptp_pub = zenoh_hal_declare_publisher(session, "openneuro/ptp/status");
  if (!g_ptp_pub) {
    printf("[PTP Publisher] Failed to declare publisher\n");
    return -1;
  }
  printf("[PTP Publisher] Initialized on 'openneuro/ptp/status'\n");
  return 0;
}

void ptp_publisher_publish_status(int64_t offset_ns, uint32_t sync_count,
                                  uint8_t state) {
  if (!g_ptp_pub)
    return;

  ptp_status_t status;
  status.offset_ns = offset_ns;
  status.sync_count = sync_count;
  status.state = state;

  zenoh_hal_put(g_ptp_pub, (uint8_t *)&status, sizeof(status));

  printf("[PTP Publisher] Published: offset=%lld ns, sync_count=%u, state=%u\n",
         offset_ns, sync_count, state);
}

void ptp_publisher_cleanup(void) {
  if (g_ptp_pub) {
    zenoh_hal_undeclare_publisher(g_ptp_pub);
    g_ptp_pub = NULL;
  }
}
