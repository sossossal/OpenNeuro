#ifndef ZENOH_HAL_H
#define ZENOH_HAL_H

#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

// 前向声明
typedef struct zenoh_session_s zenoh_session_t;
typedef struct zenoh_publisher_s zenoh_publisher_t;
typedef struct zenoh_subscriber_s zenoh_subscriber_t;

// 数据回调函数类型
typedef void (*zenoh_data_handler_t)(const uint8_t *data, size_t len,
                                     void *ctx);

/**
 * 打开 Zenoh 会话
 * @param locator 连接地址 (例如 "tcp/127.0.0.1:7447")，NULL 表示默认
 * @return 会话指针，失败返回 NULL
 */
zenoh_session_t *zenoh_hal_open(const char *locator);

/**
 * 关闭 Zenoh 会话
 * @param session 会话指针
 */
void zenoh_hal_close(zenoh_session_t *session);

/**
 * 声明发布器
 * @param s 会话指针
 * @param key 主题键 (例如 "robot/sensor/imu")
 * @return 发布器指针，失败返回 NULL
 */
zenoh_publisher_t *zenoh_hal_declare_publisher(zenoh_session_t *s,
                                               const char *key);

/**
 * 发布数据
 * @param pub 发布器指针
 * @param data 数据指针
 * @param len 数据长度
 * @return 0 成功, -1 失败
 */
int zenoh_hal_put(zenoh_publisher_t *pub, const uint8_t *data, size_t len);

/**
 * 取消发布器
 * @param pub 发布器指针
 */
void zenoh_hal_undeclare_publisher(zenoh_publisher_t *pub);

/**
 * 声明订阅器
 * @param s 会话指针
 * @param key 主题键
 * @param handler 数据回调函数
 * @param ctx 用户上下文 (传递给回调)
 * @return 订阅器指针，失败返回 NULL
 */
zenoh_subscriber_t *zenoh_hal_declare_subscriber(zenoh_session_t *s,
                                                 const char *key,
                                                 zenoh_data_handler_t handler,
                                                 void *ctx);

/**
 * 取消订阅器
 * @param sub 订阅器指针
 */
void zenoh_hal_undeclare_subscriber(zenoh_subscriber_t *sub);

#ifdef __cplusplus
}
#endif

#endif // ZENOH_HAL_H
