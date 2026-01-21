#ifndef TSN_BANDWIDTH_H
#define TSN_BANDWIDTH_H

#include "flow.h"
#include <stdbool.h>
#include <stdint.h>


// 初始化带宽管理器
void tsn_bandwidth_init(void);

// 准入控制检查 (Admission Control)
// 检查是否可以将此流添加到指定的 Traffic Class 中而不违反带宽限制
// 返回 true 表示允许，false 表示拒绝
bool tsn_bandwidth_check(tsn_traffic_class_t tc, uint32_t flow_bps);

// 提交/预留带宽
void tsn_bandwidth_reserve(tsn_traffic_class_t tc, uint32_t flow_bps);

// 释放带宽
void tsn_bandwidth_release(tsn_traffic_class_t tc, uint32_t flow_bps);

// 获取当前某类别的带宽使用率 (0-100%)
uint8_t tsn_bandwidth_get_usage(tsn_traffic_class_t tc);

#endif // TSN_BANDWIDTH_H
