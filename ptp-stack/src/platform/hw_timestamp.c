#include "hw_timestamp.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


#ifdef __linux__
#include <linux/errqueue.h>
#include <linux/if.h>
#include <linux/net_tstamp.h>
#include <linux/sockios.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#endif

int hw_timestamp_init(hw_timestamp_ctx_t *ctx, const char *iface) {
  if (!ctx || !iface) {
    return -1;
  }

  memset(ctx, 0, sizeof(hw_timestamp_ctx_t));
  strncpy(ctx->iface, iface, sizeof(ctx->iface) - 1);

  // 创建 UDP 套接字
  ctx->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (ctx->sockfd < 0) {
    perror("socket");
    return -1;
  }

  return 0;
}

int hw_timestamp_enable_tx(hw_timestamp_ctx_t *ctx) {
#ifdef __linux__
  if (!ctx || ctx->sockfd < 0) {
    return -1;
  }

  // 启用 SO_TIMESTAMPING
  int flags = SOF_TIMESTAMPING_TX_HARDWARE | SOF_TIMESTAMPING_RAW_HARDWARE |
              SOF_TIMESTAMPING_OPT_TSONLY;

  if (setsockopt(ctx->sockfd, SOL_SOCKET, SO_TIMESTAMPING, &flags,
                 sizeof(flags)) < 0) {
    perror("setsockopt SO_TIMESTAMPING TX");
    return -1;
  }

  ctx->tx_enabled = true;
  printf("[HW Timestamp] TX enabled on %s\n", ctx->iface);
  return 0;
#else
  fprintf(stderr, "[HW Timestamp] Not supported on this platform\n");
  return -1;
#endif
}

int hw_timestamp_enable_rx(hw_timestamp_ctx_t *ctx) {
#ifdef __linux__
  if (!ctx || ctx->sockfd < 0) {
    return -1;
  }

  // 配置硬件时间戳
  struct hwtstamp_config hwconfig;
  memset(&hwconfig, 0, sizeof(hwconfig));
  hwconfig.tx_type = HWTSTAMP_TX_ON;
  hwconfig.rx_filter = HWTSTAMP_FILTER_ALL;

  struct ifreq ifr;
  memset(&ifr, 0, sizeof(ifr));
  strncpy(ifr.ifr_name, ctx->iface, sizeof(ifr.ifr_name) - 1);
  ifr.ifr_data = (char *)&hwconfig;

  if (ioctl(ctx->sockfd, SIOCSHWTSTAMP, &ifr) < 0) {
    perror("ioctl SIOCSHWTSTAMP");
    fprintf(stderr,
            "[HW Timestamp] Hardware timestamping not supported on %s\n",
            ctx->iface);
    return -1;
  }

  // 启用 SO_TIMESTAMPING
  int flags = SOF_TIMESTAMPING_RX_HARDWARE | SOF_TIMESTAMPING_RAW_HARDWARE;

  if (setsockopt(ctx->sockfd, SOL_SOCKET, SO_TIMESTAMPING, &flags,
                 sizeof(flags)) < 0) {
    perror("setsockopt SO_TIMESTAMPING RX");
    return -1;
  }

  ctx->rx_enabled = true;
  printf("[HW Timestamp] RX enabled on %s\n", ctx->iface);
  return 0;
#else
  fprintf(stderr, "[HW Timestamp] Not supported on this platform\n");
  return -1;
#endif
}

int hw_timestamp_get_tx(hw_timestamp_ctx_t *ctx, uint64_t *timestamp_ns) {
#ifdef __linux__
  if (!ctx || !timestamp_ns || ctx->sockfd < 0) {
    return -1;
  }

  // 从错误队列读取时间戳
  char control[512];
  struct msghdr msg;
  struct iovec iov;
  char data[1];

  memset(&msg, 0, sizeof(msg));
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;
  iov.iov_base = data;
  iov.iov_len = sizeof(data);
  msg.msg_control = control;
  msg.msg_controllen = sizeof(control);

  int ret = recvmsg(ctx->sockfd, &msg, MSG_ERRQUEUE | MSG_DONTWAIT);
  if (ret < 0) {
    if (errno != EAGAIN && errno != EWOULDBLOCK) {
      perror("recvmsg MSG_ERRQUEUE");
    }
    return -1;
  }

  // 解析控制消息
  struct cmsghdr *cmsg;
  for (cmsg = CMSG_FIRSTHDR(&msg); cmsg; cmsg = CMSG_NXTHDR(&msg, cmsg)) {
    if (cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SCM_TIMESTAMPING) {

      struct timespec *ts = (struct timespec *)CMSG_DATA(cmsg);

      // 使用硬件时间戳 (ts[2])
      *timestamp_ns =
          (uint64_t)ts[2].tv_sec * 1000000000ULL + (uint64_t)ts[2].tv_nsec;

      ctx->last_tx_timestamp = *timestamp_ns;
      return 0;
    }
  }

  return -1;
#else
  return -1;
#endif
}

int hw_timestamp_extract_rx(hw_timestamp_ctx_t *ctx, struct msghdr *msg,
                            uint64_t *timestamp_ns) {
#ifdef __linux__
  if (!ctx || !msg || !timestamp_ns) {
    return -1;
  }

  // 解析控制消息
  struct cmsghdr *cmsg;
  for (cmsg = CMSG_FIRSTHDR(msg); cmsg; cmsg = CMSG_NXTHDR(msg, cmsg)) {
    if (cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SCM_TIMESTAMPING) {

      struct timespec *ts = (struct timespec *)CMSG_DATA(cmsg);

      // 使用硬件时间戳 (ts[2])
      *timestamp_ns =
          (uint64_t)ts[2].tv_sec * 1000000000ULL + (uint64_t)ts[2].tv_nsec;

      ctx->last_rx_timestamp = *timestamp_ns;
      return 0;
    }
  }

  return -1;
#else
  return -1;
#endif
}

void hw_timestamp_cleanup(hw_timestamp_ctx_t *ctx) {
  if (ctx && ctx->sockfd >= 0) {
    close(ctx->sockfd);
    ctx->sockfd = -1;
  }
}

bool hw_timestamp_is_supported(const char *iface) {
#ifdef __linux__
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    return false;
  }

  struct hwtstamp_config hwconfig;
  memset(&hwconfig, 0, sizeof(hwconfig));
  hwconfig.tx_type = HWTSTAMP_TX_ON;
  hwconfig.rx_filter = HWTSTAMP_FILTER_ALL;

  struct ifreq ifr;
  memset(&ifr, 0, sizeof(ifr));
  strncpy(ifr.ifr_name, iface, sizeof(ifr.ifr_name) - 1);
  ifr.ifr_data = (char *)&hwconfig;

  bool supported = (ioctl(sockfd, SIOCSHWTSTAMP, &ifr) == 0);

  close(sockfd);
  return supported;
#else
  return false;
#endif
}
