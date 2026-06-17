//
//  Addresses.cpp
//  dstd
//
//  Created by Daniel Lifshitz on 19/09/2025.
//
#include "Address.hpp"

#include <mach/arm/vm_param.h>

#include "Checkers.hpp"


namespace dstd::unstable {

namespace {

void* stripAssemblyPointer(void* assemblyPointer) {
    return reinterpret_cast<void*>(VM_KERNEL_STRIP_PTR(assemblyPointer));
}

Result<uintptr_t> decodeAdrpPageBase(void* assemblyPointer) {
    void* const instructionPointer = stripAssemblyPointer(assemblyPointer);

    const uint32_t adrpInstruction = static_cast<uint32_t*>(instructionPointer)[0];
    const uintptr_t numericAddress = reinterpret_cast<uintptr_t>(instructionPointer);

    GENERIC_CHECK(
        0x90000000 == (adrpInstruction & 0x9F000000),
        KERN_INVALID_ADDRESS,
        "First instruction is not ADRP: 0x%08X",
        adrpInstruction
    );

    int32_t immlo = (adrpInstruction >> 29) & 0x3;
    int32_t immhi = (adrpInstruction >> 5) & 0x7FFFF;
    int32_t imm = (immhi << 2) | immlo;

    if (imm & (1 << 20)) {
        imm |= ~((1 << 21) - 1);
    }

    return (numericAddress & ~0xFFFULL) + (static_cast<int64_t>(imm) << 12);
}

} // namespace

Result<void*> addressFromAdrp(void* assemblyPointer) {
    CHECK_RESULT(pageBase, decodeAdrpPageBase(assemblyPointer), "Failed to decode ADRP instruction");

    void* const instructionPointer = stripAssemblyPointer(assemblyPointer);
    const uint32_t ldrInstruction = static_cast<uint32_t*>(instructionPointer)[1];

    GENERIC_CHECK(
        0x39000000 == (ldrInstruction & 0x3B000000),
        KERN_INVALID_ADDRESS,
        "Second instruction is not 32-bit LDR: 0x%08X",
        ldrInstruction
    );

    const int imm12 = (ldrInstruction >> 10) & 0xFFF;
    return reinterpret_cast<void*>(pageBase + static_cast<uintptr_t>(imm12) * 4);
}

Result<void*> addressFromAdrl(void* assemblyPointer) {
    CHECK_RESULT(pageBase, decodeAdrpPageBase(assemblyPointer), "Failed to decode ADRP instruction");

    void* const instructionPointer = stripAssemblyPointer(assemblyPointer);
    const uint32_t addInstruction = static_cast<uint32_t*>(instructionPointer)[1];

    GENERIC_CHECK(
        0x91000000 == (addInstruction & 0xFF800000),
        KERN_INVALID_ADDRESS,
        "Second instruction is not 64-bit ADD immediate: 0x%08X",
        addInstruction
    );

    const int imm12 = (addInstruction >> 10) & 0xFFF;
    return reinterpret_cast<void*>(pageBase + static_cast<uintptr_t>(imm12));
}

} // namespace dstd::unstable
