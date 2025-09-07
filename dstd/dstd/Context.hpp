//
//  Context.hpp
//  MyKext
//
//  Created by Daniel Lifshitz on 05/09/2025.
//
#pragma once

#include "KauthCallbacks/KauthCallback.hpp"

#include "Strings/String.hpp"


struct Context {
    Context(dstd::KauthCallback<dstd::String>&& executionCallback)
        : executionCallback(dstd::move(executionCallback))
    {}
    
    dstd::KauthCallback<dstd::String> executionCallback;
};
