#ifndef NETLINK_SOCKET_H
#define NETLINK_SOCKET_H

#include <stdbool.h>
#include <stdint.h>


// Handle for Netlink connection
typedef struct nl_sock_ctx_s nl_sock_ctx_t;

// Initialization
nl_sock_ctx_t *netlink_init(void);

// Clean up
void netlink_close(nl_sock_ctx_t *ctx);

// Send a raw TC (Traffic Control) message
// returns 0 on success, <0 on error
int netlink_send_tc_msg(nl_sock_ctx_t *ctx, int family, int cmd, void *data,
                        size_t len);

// Apply Taprio configuration to interface (updates kernel Qdisc)
int netlink_apply_taprio(nl_sock_ctx_t *ctx, const char *ifname,
                         void *taprio_config);

#endif // NETLINK_SOCKET_H
