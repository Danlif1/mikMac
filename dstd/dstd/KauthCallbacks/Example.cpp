//
//  Example.cpp
//  MyKext
//
//  Created by Daniel Lifshitz on 06/09/2025.
//
#include <sys/vnode.h>

#include "Example.hpp"


/**
    Get the vnode path from a vnode pointer.
    TODO: Make a wrapper to vnode in dstd
 */
dstd::Result<dstd::String> getVnodePath(vnode_t vnode) {
    GENERIC_CHECK(nullptr != vnode, KERN_INVALID_ADDRESS, "Invalid vnode");
    
    CHECK_RESULT(longName, dstd::String::make(MAXPATHLEN), "Failed to create longName");
    
    int length = MAXPATHLEN;
    auto result = vn_getpath(vnode, longName.c_str(), &length);
    // `vn_getpath` is deprecated, it will always return an error (5) we ignore it as it still does its work.
    GENERIC_CHECK(0 != length, KERN_FAILURE, "Failed to get vnode path");
    
    // Copy the long buffer to a smaller one to save space.
    CHECK_RESULT(shortName, dstd::String::make(longName.c_str()), "Failed to create short name");

    return dstd::Result<dstd::String>::make(dstd::move(shortName));
}

dstd::Result<int> callbackImp(kauth_cred_t credential,
                              void* voidData,
                              kauth_action_t action,
                              uintptr_t context,
                              vnode_t currentVnode,
                              uintptr_t parentVnode,
                              uintptr_t error) {
    // Start by checking if the action is execution.
    GENERIC_CHECK_NO_LOG(action & KAUTH_VNODE_EXECUTE, KERN_SUCCESS);
    
    GENERIC_CHECK(nullptr != voidData, KERN_INVALID_ADDRESS, "data is nullptr");
    dstd::String* data = static_cast<dstd::String*>(voidData);
    
    // Get the vnode file path.
    CHECK_RESULT(path, getVnodePath(currentVnode), "Failed to get path");
    
    // Check if it ends with evil.
    GENERIC_CHECK_NO_LOG(nullptr != dstd::strstr(path.c_str(), data->c_str()), KERN_SUCCESS);
    
    LOG(dstd::LogLevel::LOG_INFO, "Tried executing: %s", path.c_str());
    return dstd::Result<int>::make(KAUTH_RESULT_DENY);
}

int callback(kauth_cred_t credential,
             void* voidData,
             kauth_action_t action,
             uintptr_t context,
             uintptr_t currentVnode,
             uintptr_t parentVnode,
             uintptr_t error) {
    
    auto resultStatus = callbackImp(credential, voidData, action, context, reinterpret_cast<vnode_t>(currentVnode), parentVnode, error);
    if (resultStatus.hasValue()) {
        return resultStatus.value();
    }
        
    return KAUTH_RESULT_DEFER;
}

dstd::Result<dstd::KauthCallback<dstd::String>> registerExecutionPreventorExample(dstd::String&& subString) {
    CHECK_RESULT(executionCallback,
                 dstd::KauthCallback<dstd::String>::make(KAUTH_SCOPE_VNODE, callback, dstd::move(subString)),
                 "Failed to create callback");
    return dstd::Result<dstd::KauthCallback<dstd::String>>::make(dstd::move(executionCallback));
}
