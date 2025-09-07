//
//  Example.hpp
//  MyKext
//
//  Created by Daniel Lifshitz on 06/09/2025.
//
#pragma once

#include "KauthCallback.hpp"

#include "../Strings/String.hpp"


/**
    Implement a callback preventing files which end with `evil` from being created
 */
dstd::Result<dstd::KauthCallback<dstd::String>> registerExecutionPreventorExample(dstd::String&& subString);
