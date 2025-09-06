//
//  Context.hpp
//  MyKext
//
//  Created by Daniel Lifshitz on 05/09/2025.
//
#pragma once

#include "KauthCallbacks/KauthCallback.hpp"


struct Context {
    Context(dstd::KauthCallback<void*>&& executionCallback)
        : executionCallback(dstd::move(executionCallback))
    {}
    
    dstd::KauthCallback<void*> executionCallback;
};
