//
//  Checkers.hpp
//  MyKext
//
//  Created by Daniel Lifshitz on 05/09/2025.
//

#define GENERIC_CHECK(action, errorValue) \
{ \
    if(!(action)) { \
        return dstd::Result<void>::makeError((errorValue)); \
    } \
}

#define CHECK_RESULT(resultName, action) \
    auto resultName##__result = (action); \
    if (resultName##__result.hasError()) { \
        return dstd::Result<void>::makeError(resultName##__result.error());\
    } \
    auto resultName = dstd::move(resultName##__result.value())
