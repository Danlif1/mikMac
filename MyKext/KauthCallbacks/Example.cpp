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
dstd::Result<dstd::UniquePtr<char>> getVnodePath(vnode_t vnode) {
    GENERIC_CHECK(nullptr != vnode, KERN_INVALID_ADDRESS);
    
    CHECK_RESULT(longName, dstd::UniquePtr<char>::makeArray<char>(MAXPATHLEN, '\0'));
    
    int length = MAXPATHLEN;
    auto result = vn_getpath(vnode, longName.getValue(), &length);
    // `vn_getpath` is deprecated, it will always return an error (5) we ignore it as it still does its work.
    GENERIC_CHECK(0 != length, KERN_FAILURE);
    
    // Copy the long buffer to a smaller one to save space.
    CHECK_RESULT(shortName, dstd::UniquePtr<char>::makeArray<char>(length, '\0'));
    for (size_t i = 0; i < shortName.getSize(); i++) {
        shortName.getValue()[i] = longName.getValue()[i];
    }
    
    return dstd::Result<dstd::UniquePtr<char>>::make(dstd::move(shortName));
}

int callback(
             kauth_cred_t credential,
             void* voidData,
             kauth_action_t action,
             uintptr_t context,
             uintptr_t currentVnode,
             uintptr_t parentVnode,
             uintptr_t error) {
    // Start by checking if the action is execution.
    if (!(action & KAUTH_VNODE_EXECUTE)) {
        // Not interesting.
        return KAUTH_RESULT_DEFER;
    }
    
    // Get the vnode file path.
    auto pathResult = getVnodePath(reinterpret_cast<vnode_t>(currentVnode));
    if (pathResult.hasError()) {
        LOG(5, "Failed to get path, error: %d", pathResult.error());
        return KAUTH_RESULT_DEFER;
    }
    
    auto path = dstd::move(pathResult.value());
    
    // Check if the path is too short so we won't go out of bounds.
    if (4 > path.getSize()) {
        // Too short to be our unwanted files.
        return KAUTH_RESULT_DEFER;
    }
    
    // Get the raw string.
    const char* pathString = path.getValue();
    
    // Check if it ends with evil.
    // path.getSize() is out of bounds, path.getSize() - 1 is \0.
    if ('l' == pathString[path.getSize() - 2] &&
        'i' == pathString[path.getSize() - 3] &&
        'v' == pathString[path.getSize() - 4] &&
        'e' == pathString[path.getSize() - 5]) {
        
        LOG(3, "Tried executing: %s", pathString);
        // Deny it.
        return KAUTH_RESULT_DENY;
    }
    
    return KAUTH_RESULT_DEFER;
}

dstd::Result<dstd::KauthCallback<void*>> registerExecutionPreventorExample() {
    CHECK_RESULT(executionCallback, dstd::KauthCallback<void*>::make(KAUTH_SCOPE_VNODE, callback, {}));
    return dstd::Result<dstd::KauthCallback<void*>>::make(dstd::move(executionCallback));
}
