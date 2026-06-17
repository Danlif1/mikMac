//
//  Memory.hpp
//  dstd
//
//  Created by Daniel Lifshitz on 30/08/2025.
//
#pragma once

#include "TypeTraites/Forward.hpp"


namespace dstd {

constexpr uint64_t MIN_KERNEL_ADDRESS = 0x80000000000;
constexpr uint64_t KERNEL_ADDRESS_MASK = 0xfffff80000000000;

template<typename T, typename... Args>
T* constructAt(T* location, Args&&... args) {
    return new (location) T(forward<Args>(args)...);
}

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
