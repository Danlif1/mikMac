//
//  Addresses.cpp
//  ProcessCreationPreventor
//
//  Created by Daniel Lifshitz on 19/09/2025.
//
#include "Address.hpp"

#include "../Checkers.hpp"


namespace dstd::unstable {

Result<void*> addressFromAdrp(void* assemblyPointer) {
    // remove the signature from the pointer.
    void* signatureAddress = reinterpret_cast<void*>(reinterpret_cast<size_t>(assemblyPointer) | 0xfffff80000000000);
    
    // Find all the variables.
    const auto adrpInstruction = static_cast<uint32_t*>(signatureAddress)[0];
    const auto ldrInstruction  = static_cast<uint32_t*>(signatureAddress)[1];
    const auto numericAddress  = reinterpret_cast<size_t>(signatureAddress);
    
    // --- Basic sanity checks ---
    // ADRP: opcode is 0b10000.. in bits [31:24], bits [31:24] for ADRP = 0b10000xxx
    GENERIC_CHECK(0x90000000 == (adrpInstruction & 0x9F000000), KERN_INVALID_ADDRESS, "First instruction is not ADRP: 0x%08X", adrpInstruction);
    
    // LDR (32-bit unsigned immediate, post-indexed/offset): opcode for unsigned LDR Wn = 0b[???]B9xxxxxx
    // Mask opcode bits [31:22] to check for 32-bit LDR unsigned immediate
    GENERIC_CHECK(0x39000000 == (ldrInstruction & 0x3B000000), KERN_INVALID_ADDRESS, "Second instruction is not 32-bit LDR: 0x%08X", ldrInstruction);
    
    // --- Decode ADRP ---
    int32_t immlo = (adrpInstruction >> 29) & 0x3;
    int32_t immhi = (adrpInstruction >> 5) & 0x7FFFF;
    int32_t imm   = (immhi << 2) | immlo;
    
    // Sign extend 21-bit immediate
    if (imm & (1 << 20)) {
        imm |= ~((1 << 21) - 1);
    }
    
    uintptr_t pageBase = (numericAddress & ~0xFFFULL) + ((int64_t)imm << 12);
    
    // --- Decode LDR ---
    int imm12 = (ldrInstruction >> 10) & 0xFFF; // immediate offset (imm12)
    return dstd::Result<void*>::make(reinterpret_cast<void*>(pageBase + imm12 * 4)); // multiply by 4 because 32-bit load
}

} // namespace dstd::unstable
