//
//  Basics.hpp
//  dstd
//
//  Created by Daniel Lifshitz on 05/12/2025.
//
#pragma once

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
#endif // DEBUG

#define GENERIC_CHECK_NO_LOG(action, errorValue) \
{ \
    if(!(action)) { \
        return dstd::Error((errorValue)); \
    } \
}

#define CHECK_RESULT_NO_VALUE_NO_LOG(action, errorMessage, ...) \
{ \
    auto __result = (action); \
    if (__result.hasError()) { \
        return dstd::Error(__result.error());\
    } \
}

#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

namespace dstd {

template<typename T>
T max(T first, T second) {
    return first > second ? first : second;
}

template<typename T>
T min(T first, T second) {
    return first < second ? first : second;
}

} // namespace dstd
