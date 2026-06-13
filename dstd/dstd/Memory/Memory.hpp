//
//  Memory.hpp
//  dstd
//
//  Created by Daniel Lifshitz on 30/08/2025.
//
#pragma once


namespace dstd {

constexpr uint64_t MIN_KERNEL_ADDRESS = 0x80000000000;
constexpr uint64_t KERNEL_ADDRESS_MASK = 0xfffff80000000000;

} // namespace dstd

inline void* operator new(size_t, void* p) noexcept {
    return p;
}

inline void operator delete(void*, void*) noexcept {
}

template<typename T>
struct DefaultDeleter {
    void operator()(T* object) {
        delete object;
    }
};

template<typename T>
struct ArrayDeleter {
    void operator()(T* object) {
        delete[] object;
    }
};
