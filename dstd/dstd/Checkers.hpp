//
//  Checkers.hpp
//  dstd
//
//  Created by Daniel Lifshitz on 05/09/2025.
//
#pragma once

#include "Logger.hpp"

#ifdef DEBUG
#include <IOKit/IOLib.h>

#define BREAK_IF_DEBUGGER_PRESENT(condition) \
{ \
    if (!(condition)) { \
        /*Needs to implement: Debugger("Check failed: " #condition);*/ \
    } \
}
#else
#define BREAK_IF_DEBUGGER_PRESENT(condition)
#endif


#define GENERIC_CHECK_NO_LOG(action, errorValue) \
{ \
    if(!(action)) { \
        return dstd::Result<void>::makeError((errorValue)); \
    } \
}

#define GENERIC_CHECK(action, errorValue, errorMessage, ...) \
{ \
    if(!(action)) { \
        LOG(dstd::LogLevel::LOG_ERROR, "error value: %d: " errorMessage, errorValue, ##__VA_ARGS__); \
        return dstd::Result<void>::makeError((errorValue)); \
    } \
}

#define CHECK_RESULT(resultName, action, errorMessage, ...) \
auto resultName##__result = (action); \
if (resultName##__result.hasError()) { \
    LOG(dstd::LogLevel::LOG_ERROR, "Action failed, error value: %d: " errorMessage, resultName##__result.error(), ##__VA_ARGS__); \
    return dstd::Result<void>::makeError(resultName##__result.error());\
} \
auto resultName = dstd::move(resultName##__result.value())

#define CHECK_RESULT_NO_VALUE(action, errorMessage, ...) \
{ \
    auto __result = (action); \
    if (__result.hasError()) { \
        LOG(dstd::LogLevel::LOG_ERROR, "Action failed, error value: %d: " errorMessage, __result.error(), ##__VA_ARGS__); \
        return dstd::Result<void>::makeError(__result.error());\
    } \
}
