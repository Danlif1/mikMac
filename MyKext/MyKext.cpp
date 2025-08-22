//
//  MyKext.c
//  MyKext
//
//  Created by Daniel Lifshitz on 14/06/2025.
//
#include <sys/systm.h>
#include <mach/mach_types.h>
 
#include "Logger.h"


extern "C" kern_return_t MyKext_start (kmod_info_t * ki, void * d)
{
    LOG(0, "starting with info ki: %p, d: %p", ki, d);
    return KERN_SUCCESS;
}
 
extern "C" kern_return_t MyKext_stop (kmod_info_t * ki, void * d)
{
    LOG(1, "stopping");
    LOG(2, "Also stopping with value: ki = %p", ki);
    printf("MyKext has stopped.\n");
    return KERN_SUCCESS;
}
