/* SPDX-License-Identifier: Proprietary
 *
 * Yunhe Enmo (Beijing) Information Technology Co., Ltd.
 * Copyright (c) 2020 , All rights reserved.
 *
 */

#ifndef _H_DDKV_LOG_2021_02_10_19_35_37
#define _H_DDKV_LOG_2021_02_10_19_35_37

#include <Singleton.h>
#include <logger/Logger.h>

class Log : public ws::Singleton<Log> {
public:
	Log() {
		_logger.reset(new ws::Logger(
			"ddkv",
			ws::kLoggerSinkTypeConsole |
			ws::kLoggerSinkTypeFile,
			false));
	}

	~Log() {
		_logger = nullptr;
	}

public:
	ws::LoggerPtr _logger;
};

#define MLOG(l, p, ...) \
	ws::writeLogger(Log::getInstance()->_logger, l, \
		__FILE__, __LINE__, p, ##__VA_ARGS__)

#ifdef _DEBUG
#define MLOGD(p, ...) MLOG(ws::kLoggerLevelDebug, p, ##__VA_ARGS__)
#else
#define MLOGD(p, ...) ((void)0)
#endif

#define MLOGT(p, ...) MLOG(ws::kLoggerLevelTrace, p, ##__VA_ARGS__)
#define MLOGI(p, ...) MLOG(ws::kLoggerLevelInfo, p, ##__VA_ARGS__)
#define MLOGW(p, ...) MLOG(ws::kLoggerLevelWarn, p, ##__VA_ARGS__)
#define MLOGE(p, ...) MLOG(ws::kLoggerLevelError, p, ##__VA_ARGS__)

#endif//_H_DDKV_LOG_2021_02_10_19_35_37

