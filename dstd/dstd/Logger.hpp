//
//  Logger.h
//  MyKext
//
//  Created by Daniel Lifshitz on 16/07/2025.
//
#pragma once

#include "Checkers.hpp"

#include "DataStructures/Singleton.hpp"
#include "Files/FileLogger.hpp"


#define DRIVER_ID "MyKext:"

#define FILE_LOG(fileLogger, logLevel, message, ...) \
    (fileLogger).log( \
        logLevel, \
        "file: %s, function: %s, line %d " message "\n", \
        __FILE__, \
        __func__, \
        __LINE__, \
        ##__VA_ARGS__)

#define LOG(logLevel, message, ...) \
{ \
    printf(DRIVER_ID " level: %d, file: %s, function: %s, line %d " message "\n" , \
        logLevel, \
        __FILE__, \
        __func__, \
        __LINE__, \
        ##__VA_ARGS__);
    auto loggerResult = dstd::Singleton<dstd::FileLogger>::get(); \
    if (loggerResult.hasValue()) { \
        FILE_LOG(loggerResult.value(), logLevel, message, ##__VA_ARGS__); \
    }
}

namespace dstd {

enum LogLevel {
    LOG_DEBUG = 0,
    LOG_INFO = 1,
    LOG_WARNING = 2,
    LOG_ERROR = 3,
    LOG_CRITICAL = 4,
};

} // namespace dstd
