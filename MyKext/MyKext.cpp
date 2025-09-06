//
//  MyKext.c
//  MyKext
//
//  Created by Daniel Lifshitz on 14/06/2025.
//
#include <sys/systm.h>
#include <sys/vnode.h>
#include <mach/mach_types.h>
 
 
#include "Context.hpp"

#include "Logger.hpp"
#include "Result.hpp"

#include "Pointers/UniquePtr.hpp"
#include "Pointers/SharedPtr.hpp"

#include "KauthCallbacks/KauthCallback.hpp"
#include "KauthCallbacks/Example.hpp"

Context* g_context = nullptr;


dstd::Result<void> startImp(kmod_info_t * kmodInfo, void * data) {
    CHECK_RESULT(subString, dstd::String::make("evil"), "Failed to create sub string");
    CHECK_RESULT(executionCallback, registerExecutionPreventorExample(dstd::move(subString)), "Failed to create callback");
    g_context = new Context(dstd::move(executionCallback));
    
    return {};
}

extern "C" kern_return_t MyKext_start (kmod_info_t * kmodInfo, void * data)
{
    LOG(dstd::LogLevel::LOG_DEBUG, "starting with info kmodInfo: %p, data: %p", kmodInfo, data);
    return startImp(kmodInfo, data);
}
 
extern "C" kern_return_t MyKext_stop (kmod_info_t * kmodInfo, void * data)
{
    if (nullptr != g_context) {
        delete g_context;
        g_context = nullptr;
    }
    
    LOG(1, "stopping");
    LOG(2, "Also stopping with value: kmodInfo = %p", kmodInfo);
    printf("MyKext has stopped.\n");
    return KERN_SUCCESS;
}
