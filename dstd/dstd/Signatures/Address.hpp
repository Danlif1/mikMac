//
//  Addresses.hpp
//  ProcessCreationPreventor
//
//  Created by Daniel Lifshitz on 19/09/2025.
//
#pragma once

#include "../Result.hpp"


namespace dstd::unstable {

Result<void*> addressFromAdrp(void* assemblyPointer);

} // namespace dstd::unstable
