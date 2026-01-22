#include "delay.h"
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

// Define macros if not already defined
#ifndef NTOHS
#define NTOHS(x) ntohs(x)
#endif
#ifndef HTONS
#define HTONS(x) htons(x)
#endif
#ifndef HTONL
#define HTONL(x) htonl(x)
#endif

void ptp_delay_handle_req(ptp_master_ctx_t *ctx,
                          const ptp_delay_req_message_t *req, uint64_t rx_ts) {
  if (!ctx || !req)
    return;

  // 1. 验证 Sequence ID (通常不需要严格匹配 SYNC，但在某些 Profile 中有要求)
  uint16_t seq_id_raw = req->header.sequence_id;
  uint16_t seq_id = NTOHS(seq_id_raw);

  // 2. 提取请求者的 Port Identity
  // const uint8_t *requestor = req->header.source_port_identity;

  // 3. 构建 DELAY_RESP
  ptp_delay_resp_message_t resp;
  memset(&resp, 0, sizeof(resp));

  ptp_msg_build_header(&resp.header, PTP_MSG_DELAY_RESP,
                       sizeof(ptp_delay_resp_message_t));

  // 设置头信息
  memcpy(resp.header.source_port_identity, ctx->port_identity, 10);
  resp.header.sequence_id =
      req->header.sequence_id; // 必须匹配请求的 Sequence ID
  resp.header.domain_number = ctx->config.domain_number;
  resp.header.log_message_interval = 0x7F; // Response 不需要 interval

  // 4. 填充接收时间戳 (Rx Timestamp of DELAY_REQ)
  // 需要转换为 Big Endian
  uint16_t sec_msb = (uint16_t)((rx_ts >> 48) & 0xFFFF);
  uint32_t sec_lsb = (uint32_t)((rx_ts >> 16) & 0xFFFFFFFF);
  uint32_t ns = (uint32_t)(rx_ts & 0xFFFF);

  resp.receive_timestamp.seconds_msb = HTONS(sec_msb);
  resp.receive_timestamp.seconds_lsb = HTONL(sec_lsb);
  resp.receive_timestamp.nanoseconds = HTONL(ns);

  // 5. 复制请求者 ID
  memcpy(resp.requesting_port_identity, req->header.source_port_identity, 10);

  // 6. 发送响应 (Mock)
  // ptp_net_send(&resp);
  printf("[PTP DELAY] Sent DELAY_RESP to Seq=%d, RxTS=%llu\n", seq_id, rx_ts);
}
