//
//  Checkers.hpp
//  MyKext
//
//  Created by Daniel Lifshitz on 05/09/2025.
//
#pragma once

#include "Logger.h"


#define GENERIC_CHECK(action, errorValue) \
{ \
    if(!(action)) { \
        LOG(5, "Action failed, error value: %d", errorValue); \
        return dstd::Result<void>::makeError((errorValue)); \
    } \
}

#define CHECK_RESULT(resultName, action) \
    auto resultName##__result = (action); \
    if (resultName##__result.hasError()) { \
        LOG(5, "Action failed, error value: %d", resultName##__result.error()); \
        return dstd::Result<void>::makeError(resultName##__result.error());\
    } \
    auto resultName = dstd::move(resultName##__result.value())
