//
//  Example.hpp
//  MyKext
//
//  Created by Daniel Lifshitz on 06/09/2025.
//
#pragma once

#include "KauthCallback.hpp"


/**
    Implement a callback preventing files which end with `evil` from being created
 */
dstd::Result<dstd::KauthCallback<void*>> registerExecutionPreventorExample();
