#ifndef PTP_HW_TIMESTAMP_H
#define PTP_HW_TIMESTAMP_H

#include <stdbool.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

/**
 * 硬件时间戳上下文
 */
typedef struct {
  int sockfd;                 // 网络套接字
  char iface[16];             // 网络接口名
  bool tx_enabled;            // TX 时间戳是否启用
  bool rx_enabled;            // RX 时间戳是否启用
  uint64_t last_tx_timestamp; // 最后一次 TX 时间戳
  uint64_t last_rx_timestamp; // 最后一次 RX 时间戳
} hw_timestamp_ctx_t;

/**
 * 初始化硬件时间戳
 * @param ctx 时间戳上下文
 * @param iface 网络接口名 (例如 "eth0")
 * @return 0 成功, -1 失败
 */
int hw_timestamp_init(hw_timestamp_ctx_t *ctx, const char *iface);

/**
 * 启用 TX 硬件时间戳
 * @param ctx 时间戳上下文
 * @return 0 成功, -1 失败
 */
int hw_timestamp_enable_tx(hw_timestamp_ctx_t *ctx);

/**
 * 启用 RX 硬件时间戳
 * @param ctx 时间戳上下文
 * @return 0 成功, -1 失败
 */
int hw_timestamp_enable_rx(hw_timestamp_ctx_t *ctx);

/**
 * 获取 TX 时间戳 (从错误队列)
 * @param ctx 时间戳上下文
 * @param timestamp_ns 输出: 时间戳 (纳秒)
 * @return 0 成功, -1 失败
 */
int hw_timestamp_get_tx(hw_timestamp_ctx_t *ctx, uint64_t *timestamp_ns);

/**
 * 从接收的消息中提取 RX 时间戳
 * @param ctx 时间戳上下文
 * @param msg msghdr 结构 (包含控制消息)
 * @param timestamp_ns 输出: 时间戳 (纳秒)
 * @return 0 成功, -1 失败
 */
int hw_timestamp_extract_rx(hw_timestamp_ctx_t *ctx, struct msghdr *msg,
                            uint64_t *timestamp_ns);

/**
 * 清理硬件时间戳资源
 * @param ctx 时间戳上下文
 */
void hw_timestamp_cleanup(hw_timestamp_ctx_t *ctx);

/**
 * 检查硬件时间戳是否支持
 * @param iface 网络接口名
 * @return true 支持, false 不支持
 */
bool hw_timestamp_is_supported(const char *iface);

#ifdef __cplusplus
}
#endif

#endif // PTP_HW_TIMESTAMP_H
