//
//  Logger.h
//  MyKext
//
//  Created by Daniel Lifshitz on 16/07/2025.
//
#pragma once

#define DRIVER_ID "MyKext:"

#define LOG(logLevel, message, ...) \
printf(DRIVER_ID " level: %d, file: %s, function: %s, line %d " message "\n" , \
       logLevel, \
       __FILE__, \
       __func__, \
       __LINE__, \
       ##__VA_ARGS__)


namespace dstd {

enum LogLevel {
    LOG_DEBUG = 0,
    LOG_INFO = 1,
    LOG_WARNING = 2,
    LOG_ERROR = 3,
    LOG_CRITICAL = 4,
};

} // namespace dstd
