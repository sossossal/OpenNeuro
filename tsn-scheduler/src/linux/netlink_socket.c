#include "netlink_socket.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#ifdef __linux__
#include <linux/if.h>
#include <linux/pkt_sched.h>
#include <linux/rtnetlink.h>
#include <net/if.h>
#include <sys/socket.h>
#include <unistd.h>


// Netlink 辅助函数
static int addattr_l(struct nlmsghdr *n, int maxlen, int type, const void *data,
                     int alen) {
  int len = RTA_LENGTH(alen);
  struct rtattr *rta;

  if (NLMSG_ALIGN(n->nlmsg_len) + RTA_ALIGN(len) > maxlen) {
    fprintf(stderr, "addattr_l: message exceeded bound of %d\n", maxlen);
    return -1;
  }

  rta = NLMSG_TAIL(n);
  rta->rta_type = type;
  rta->rta_len = len;
  if (alen) {
    memcpy(RTA_DATA(rta), data, alen);
  }
  n->nlmsg_len = NLMSG_ALIGN(n->nlmsg_len) + RTA_ALIGN(len);
  return 0;
}

static struct rtattr *addattr_nest(struct nlmsghdr *n, int maxlen, int type) {
  struct rtattr *nest = NLMSG_TAIL(n);
  addattr_l(n, maxlen, type, NULL, 0);
  return nest;
}

static int addattr_nest_end(struct nlmsghdr *n, struct rtattr *nest) {
  nest->rta_len = (char *)NLMSG_TAIL(n) - (char *)nest;
  return n->nlmsg_len;
}

int netlink_socket_init(nl_sock_ctx_t *ctx) {
  if (!ctx) {
    return -1;
  }

  memset(ctx, 0, sizeof(nl_sock_ctx_t));

  // 创建 Netlink 套接字
  ctx->fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
  if (ctx->fd < 0) {
    perror("socket(AF_NETLINK)");
    return -1;
  }

  // 绑定
  struct sockaddr_nl local;
  memset(&local, 0, sizeof(local));
  local.nl_family = AF_NETLINK;

  if (bind(ctx->fd, (struct sockaddr *)&local, sizeof(local)) < 0) {
    perror("bind(netlink)");
    close(ctx->fd);
    return -1;
  }

  printf("[Netlink] Socket initialized\n");
  return 0;
}

int netlink_apply_taprio(nl_sock_ctx_t *ctx, const char *ifname,
                         const tsn_config_t *config) {
  if (!ctx || !ifname || !config) {
    return -1;
  }

  // 获取接口索引
  unsigned int ifindex = if_nametoindex(ifname);
  if (ifindex == 0) {
    fprintf(stderr, "Interface %s not found\n", ifname);
    return -1;
  }

  // 构建 Netlink 消息
  struct {
    struct nlmsghdr n;
    struct tcmsg t;
    char buf[4096];
  } req;

  memset(&req, 0, sizeof(req));

  req.n.nlmsg_len = NLMSG_LENGTH(sizeof(struct tcmsg));
  req.n.nlmsg_type = RTM_NEWQDISC;
  req.n.nlmsg_flags = NLM_F_REQUEST | NLM_F_CREATE | NLM_F_EXCL;
  req.n.nlmsg_seq = ++ctx->seq;

  req.t.tcm_family = AF_UNSPEC;
  req.t.tcm_ifindex = ifindex;
  req.t.tcm_handle = 0x10000; // 1:0
  req.t.tcm_parent = TC_H_ROOT;

  // 设置 qdisc 类型为 taprio
  addattr_l(&req.n, sizeof(req), TCA_KIND, "taprio", strlen("taprio") + 1);

  // 添加 taprio 选项
  struct rtattr *tail = addattr_nest(&req.n, sizeof(req), TCA_OPTIONS);

  // num_tc (流量类别数)
  addattr_l(&req.n, sizeof(req), TCA_TAPRIO_ATTR_PRIOMAP, &config->num_tc,
            sizeof(config->num_tc));

  // 队列映射
  for (int i = 0; i < config->num_tc; i++) {
    struct tc_mqprio_qopt qopt;
    memset(&qopt, 0, sizeof(qopt));
    qopt.num_tc = config->num_tc;

    // 每个 TC 一个队列
    for (int j = 0; j < config->num_tc; j++) {
      qopt.count[j] = 1;
      qopt.offset[j] = j;
    }

    addattr_l(&req.n, sizeof(req), TCA_TAPRIO_ATTR_SCHED_ENTRY_LIST, &qopt,
              sizeof(qopt));
  }

  // base-time (基准时间)
  addattr_l(&req.n, sizeof(req), TCA_TAPRIO_ATTR_SCHED_BASE_TIME,
            &config->base_time, sizeof(config->base_time));

  // cycle-time (周期时间)
  addattr_l(&req.n, sizeof(req), TCA_TAPRIO_ATTR_SCHED_CYCLE_TIME,
            &config->cycle_time, sizeof(config->cycle_time));

  // 门控列表
  struct rtattr *entry_list =
      addattr_nest(&req.n, sizeof(req), TCA_TAPRIO_ATTR_SCHED_ENTRY_LIST);

  for (int i = 0; i < config->num_gates; i++) {
    struct rtattr *entry =
        addattr_nest(&req.n, sizeof(req), TCA_TAPRIO_ATTR_SCHED_ENTRY);

    // gate_mask (哪些队列开放)
    addattr_l(&req.n, sizeof(req), TCA_TAPRIO_SCHED_ENTRY_GATE_MASK,
              &config->gates[i].gate_mask, sizeof(uint32_t));

    // interval (时间段持续时间)
    addattr_l(&req.n, sizeof(req), TCA_TAPRIO_SCHED_ENTRY_INTERVAL,
              &config->gates[i].duration, sizeof(uint32_t));

    addattr_nest_end(&req.n, entry);
  }

  addattr_nest_end(&req.n, entry_list);
  addattr_nest_end(&req.n, tail);

  // 发送到内核
  if (send(ctx->fd, &req, req.n.nlmsg_len, 0) < 0) {
    perror("send(netlink)");
    return -1;
  }

  // 接收响应
  char buf[4096];
  int len = recv(ctx->fd, buf, sizeof(buf), 0);
  if (len < 0) {
    perror("recv(netlink)");
    return -1;
  }

  struct nlmsghdr *h = (struct nlmsghdr *)buf;
  if (h->nlmsg_type == NLMSG_ERROR) {
    struct nlmsgerr *err = (struct nlmsgerr *)NLMSG_DATA(h);
    if (err->error != 0) {
      fprintf(stderr, "Netlink error: %s\n", strerror(-err->error));
      return -1;
    }
  }

  printf("[Netlink] Taprio applied to %s\n", ifname);
  return 0;
}

int netlink_get_taprio_status(nl_sock_ctx_t *ctx, const char *ifname,
                              tsn_status_t *status) {
  if (!ctx || !ifname || !status) {
    return -1;
  }

  // 获取接口索引
  unsigned int ifindex = if_nametoindex(ifname);
  if (ifindex == 0) {
    return -1;
  }

  // 构建查询消息
  struct {
    struct nlmsghdr n;
    struct tcmsg t;
    char buf[1024];
  } req;

  memset(&req, 0, sizeof(req));

  req.n.nlmsg_len = NLMSG_LENGTH(sizeof(struct tcmsg));
  req.n.nlmsg_type = RTM_GETQDISC;
  req.n.nlmsg_flags = NLM_F_REQUEST;
  req.n.nlmsg_seq = ++ctx->seq;

  req.t.tcm_family = AF_UNSPEC;
  req.t.tcm_ifindex = ifindex;

  // 发送查询
  if (send(ctx->fd, &req, req.n.nlmsg_len, 0) < 0) {
    perror("send(netlink query)");
    return -1;
  }

  // 接收响应
  char buf[4096];
  int len = recv(ctx->fd, buf, sizeof(buf), 0);
  if (len < 0) {
    perror("recv(netlink query)");
    return -1;
  }

  // 解析响应 (简化版)
  memset(status, 0, sizeof(tsn_status_t));
  status->active = true;
  strncpy(status->ifname, ifname, sizeof(status->ifname) - 1);

  return 0;
}

void netlink_socket_cleanup(nl_sock_ctx_t *ctx) {
  if (ctx && ctx->fd >= 0) {
    close(ctx->fd);
    ctx->fd = -1;
  }
}

#else
// 非 Linux 平台的 Stub 实现
int netlink_socket_init(nl_sock_ctx_t *ctx) {
  fprintf(stderr, "[Netlink] Not supported on this platform\n");
  return -1;
}

int netlink_apply_taprio(nl_sock_ctx_t *ctx, const char *ifname,
                         const tsn_config_t *config) {
  fprintf(stderr, "[Netlink] Taprio not supported on this platform\n");
  return -1;
}

int netlink_get_taprio_status(nl_sock_ctx_t *ctx, const char *ifname,
                              tsn_status_t *status) {
  return -1;
}

void netlink_socket_cleanup(nl_sock_ctx_t *ctx) {
  // No-op
}
#endif
