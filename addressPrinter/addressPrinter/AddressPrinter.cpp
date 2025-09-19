//
//  AddressPrinter.cpp
//  AddressPrinter
//
//  Created by Daniel Lifshitz on 19/09/2025.
//
#include <mach/mach_types.h>

#include "dstd/Checkers.hpp"

#include "dstd/Signatures/Address.hpp"


extern "C" void OSReportWithBacktrace(const char* format, ...);

dstd::Result<void> startImp(kmod_info_t * kmodInfo, void * data) {
    // The address where OSReportWithBacktrace accesses doprnt_hide_pointers.
    const auto address = reinterpret_cast<void*>(reinterpret_cast<size_t>(OSReportWithBacktrace) + 0x15c);
    
    CHECK_RESULT(hidePointersAddress, dstd::unstable::addressFromAdrp(address), "Failed to reconstruct address");
    auto hidePointersPointer = static_cast<boolean_t*>(hidePointersAddress);
    
    // Setting it to not hide.
    *hidePointersPointer = false;
    LOG(dstd::LogLevel::LOG_DEBUG, "Should print kmodInfo real address: %p", kmodInfo);
    
    return {};
}

extern "C" kern_return_t AddressPrinter_start(kmod_info_t* kmodInfo, void* data) {
    LOG(dstd::LogLevel::LOG_DEBUG, "Should not print kmodInfo real address: %p", kmodInfo);
    return startImp(kmodInfo, data);
}

extern "C" kern_return_t AddressPrinter_stop(kmod_info_t *kmodInfo, void* data) {
    return KERN_SUCCESS;
}
