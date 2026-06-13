//
//  tests.cpp
//  tests
//
//  Created by Daniel Lifshitz on 13/06/2026.
//
#include <mach/mach_types.h>

#include "Framework/TestFramework.hpp"
#include "Result.hpp"
#include "Signatures/Address.hpp"



using namespace dstd;
extern "C" void OSReportWithBacktrace(const char* format, ...);

Result<void> testsStartImp(kmod_info_t* kmodInfo, void* data) {
    (void)kmodInfo;
    (void)data;
    const auto address = reinterpret_cast<void*>(reinterpret_cast<size_t>(OSReportWithBacktrace) + 0x15c);
    
    CHECK_RESULT(hidePointersAddress, dstd::unstable::addressFromAdrp(address), "Failed to reconstruct address");
    auto hidePointersPointer = static_cast<boolean_t*>(hidePointersAddress);
    
    // Setting it to not hide.
    *hidePointersPointer = false;
    LOG(dstd::LogLevel::LOG_DEBUG, "Should print kmodInfo real address: %p", kmodInfo);
    
    LOG(LogLevel::LOG_INFO, "tests: starting dstd unit tests");
    CHECK_RESULT_NO_VALUE(tests::runAllTests(), "dstd unit tests failed");
    LOG(LogLevel::LOG_INFO, "tests: finished running dstd unit tests");

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
