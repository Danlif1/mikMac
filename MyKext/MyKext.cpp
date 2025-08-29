//
//  MyKext.c
//  MyKext
//
//  Created by Daniel Lifshitz on 14/06/2025.
//
#include <sys/systm.h>
#include <mach/mach_types.h>
 
#include "Logger.h"
#include "Optional.hpp"


int* g_int;

dstd::Optional<int> function(bool shouldReturn, int toReturn) {
    if (shouldReturn) {
        return dstd::Optional<int>(toReturn);
    } else {
        return {};
    }
}

extern "C" kern_return_t MyKext_start (kmod_info_t * ki, void * d)
{
    function(true, 5);
    auto first = function(true, 7);
    auto second = function(false, 8);
    
    if (first.isValid()) {
        auto fullFirst = first.get();
        LOG(0, "first %d", *fullFirst);
    }
    
    if (!second.isValid()) {
        LOG(0, "second invalid");
    }
    
    g_int = new int(5);
    if (g_int) {
        size_t x = reinterpret_cast<size_t>(g_int);
        
        while (x > 0) {
            LOG(0, "%d", x % 10);
            x /= 10;
        }
        LOG(0, "new works, p: %llu, d: %d", g_int, *g_int);
    }
    LOG(0, "starting with info ki: %p, d: %p", ki, d);
    return KERN_SUCCESS;
}
 
extern "C" kern_return_t MyKext_stop (kmod_info_t * ki, void * d)
{
    if (nullptr != g_int) {
        LOG(0, "g_int: %llu, value %d", g_int, *g_int);
        delete g_int;
    }
    
    LOG(1, "stopping");
    LOG(2, "Also stopping with value: ki = %p", ki);
    printf("MyKext has stopped.\n");
    return KERN_SUCCESS;
}
