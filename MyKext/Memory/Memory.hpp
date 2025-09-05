//
//  Memory.hpp
//  MyKext
//
//  Created by Daniel Lifshitz on 30/08/2025.
//
#pragma once

// Placement new
inline void* operator new(size_t, void* p) noexcept {
    return p;
}

// Placement delete (only used if constructor throws during placement new)
inline void operator delete(void*, void*) noexcept {
    // do nothing
}

template<typename T>
struct DefaultDeleter {
    void operator()(T* object)
    {}
};
