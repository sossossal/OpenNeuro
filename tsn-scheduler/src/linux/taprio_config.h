#ifndef TAPRIO_CONFIG_H
#define TAPRIO_CONFIG_H

#include "../core/scheduler.h"
#include "taprio_if.h"

// 将核心调度器配置转换为 Linux Taprio 配置
// Caller must free the returned structure
struct tc_taprio_qopt_offload *
taprio_config_from_scheduler(const tsn_scheduler_t *sched);

#endif // TAPRIO_CONFIG_H
