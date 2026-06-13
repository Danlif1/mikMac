//
//  tests.cpp
//  tests
//
//  Created by Daniel Lifshitz on 13/06/2026.
//
#include <mach/mach_types.h>

#include "Framework/TestFramework.hpp"
#include "Result.hpp"


using namespace dstd;

Result<void> testsStartImp(kmod_info_t* kmodInfo, void* data) {
    (void)kmodInfo;
    (void)data;

    LOG(LogLevel::LOG_INFO, "tests: starting dstd unit tests");
    CHECK_RESULT_NO_VALUE(tests::runAllTests(), "dstd unit tests failed");

    return Result<void>::make();
}

extern "C" kern_return_t testsStart(kmod_info_t* kmodInfo, void* data) {
    return testsStartImp(kmodInfo, data);
}

extern "C" kern_return_t testsStop(kmod_info_t* kmodInfo, void* data) {
    (void)kmodInfo;
    (void)data;

    return KERN_SUCCESS;
}
