//
//  Checkers.hpp
//  dstd
//
//  Created by Daniel Lifshitz on 05/09/2025.
//
#pragma once

#include "Basics.hpp"
#include "Logger.hpp"


#define GENERIC_CHECK(action, errorValue, errorMessage, ...) \
{ \
    if(!(action)) { \
        LOG(dstd::LogLevel::LOG_ERROR, "error value: %d: " errorMessage, errorValue, ##__VA_ARGS__); \
        return dstd::Error((errorValue)); \
    } \
}

#define CHECK_RESULT(resultName, action, errorMessage, ...) \
    auto resultName##__result = (action); \
    if (resultName##__result.hasError()) { \
        LOG(dstd::LogLevel::LOG_ERROR, "Action failed, error value: %d: " errorMessage, resultName##__result.error(), ##__VA_ARGS__); \
        return dstd::Error(resultName##__result.error()); \
    } \
    auto resultName = dstd::move(resultName##__result.value())

#define CHECK_RESULT_NO_VALUE(action, errorMessage, ...) \
{ \
    auto __result = (action); \
    if (__result.hasError()) { \
        LOG(dstd::LogLevel::LOG_ERROR, "Action failed, error value: %d: " errorMessage, __result.error(), ##__VA_ARGS__); \
        return dstd::Error(__result.error());\
    } \
}
