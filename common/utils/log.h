#ifndef __LOG_H__
#define __LOG_H__

#include "spdlog/spdlog.h"
#include "spdlog/cfg/env.h"

#define BUFFER_SIZE 512

#ifndef __LOGTAG__
#define __LOGTAG__ "UNDEFINED"
#endif

#ifndef LOGI
#define LOGI(fmt, ...) spdlog::info("[" __LOGTAG__ "] {}():{} " fmt, __func__, __LINE__, ##__VA_ARGS__)
#endif

#ifndef LOGD
#define LOGD(fmt, ...) spdlog::debug("[" __LOGTAG__ "] {}():{} " fmt, __func__, __LINE__, ##__VA_ARGS__)
#endif

#ifndef LOGE
#define LOGE(fmt, ...) spdlog::error("[" __LOGTAG__ "] {}():{} " fmt, __func__, __LINE__, ##__VA_ARGS__)
#endif

#endif