//
//  Context.hpp
//  MyKext
//
//  Created by Daniel Lifshitz on 05/09/2025.
//
#pragma once

#include "dstd/KauthCallbacks/KauthCallback.hpp"

#include "dstd/Strings/String.hpp"


struct Context {
    Context(dstd::KauthCallback<dstd::String>&& executionCallback)
        : executionCallback(dstd::move(executionCallback))
    {}
    
    dstd::KauthCallback<dstd::String> executionCallback;
};
