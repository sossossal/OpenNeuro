#ifndef PTP_DELAY_H
#define PTP_DELAY_H

#include "../master/master.h"

// 处理接收到的 DELAY_REQ 消息
// rx_ts 是接收到该包的精确时间戳
void ptp_delay_handle_req(ptp_master_ctx_t *ctx,
                          const ptp_delay_req_message_t *req, uint64_t rx_ts);

#endif // PTP_DELAY_H
