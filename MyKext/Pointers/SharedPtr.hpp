//
//  SharedPtr.hpp
//  MyKext
//
//  Created by Daniel Lifshitz on 04/09/2025.
//
#pragma once

#include "../Checkers.hpp"
#include "../Result.hpp"

#include <libkern/OSAtomic.h>

namespace dstd {

template<typename T, typename Deleter = DefaultDeleter<T>>
class SharedPtr {
public:
    template<typename... Args>
    static Result<SharedPtr<T>> make(Args... args) {
        return makeArray(1, args...);
    }
    
    template<typename... Args>
    static Result<SharedPtr<T>> makeArray(const size_t size, Args... args) {
        return makeArrayWithDeleter(DefaultDeleter<T>(), size, args...);
    }
    
    template<typename... Args>
    static Result<SharedPtr<T, Deleter>> makeArrayWithDeleter(Deleter deleter, const size_t size, Args... args) {
        volatile auto counter = new int64_t(1);
        GENERIC_CHECK(nullptr != counter, KERN_NO_SPACE, "Failed to allocate memory for counter");
        
        void* raw = operator new[](size * sizeof(T));
        if (nullptr == raw) {
            delete counter;
            LOG(5, "Failed to allocate memory for sharedPtr");
            return Result<void>::makeError(KERN_NO_SPACE);
        }
        
        auto array = static_cast<T*>(raw);
        
        for (size_t i = 0; i < size; i++) {
            new (&array[i]) T(args...);
        }
        
        SharedPtr result(array, size, deleter, counter);
        return Result<SharedPtr<T, Deleter>>::make(move(result));
    }
    
    SharedPtr(SharedPtr& other)
        : m_value(other.m_value)
        , m_size(other.m_size)
        , m_deleter(other.m_deleter)
        , m_counter(other.m_counter)
    {
        increment();
    }
    
    SharedPtr& operator=(const SharedPtr& other) {
        if (this == &other) {
            return *this;
        }
        
        reset();
        
        m_value = other.m_value;
        m_size = other.m_size;
        m_deleter = other.m_deleter;
        m_counter = other.m_counter;
        
        increment();
    }
    
    SharedPtr(SharedPtr&& other)
        : m_value(other.m_value)
        , m_size(other.m_size)
        , m_deleter(move(other.m_deleter))
        , m_counter(other.m_counter)
    {
        other.m_value = nullptr;
        other.m_size = 0;
        other.m_counter = nullptr;
    }
    
    SharedPtr& operator=(SharedPtr&& other) {
        if (this == &other) {
            return *this;
        }
        
        reset();
        
        m_value = other.m_value;
        m_size = other.m_size;
        m_deleter = move(other.m_deleter);
        m_counter = other.m_counter;
        
        other.m_value = nullptr;
        other.m_size = 0;
        other.m_counter = nullptr;
    }
    
    ~SharedPtr() {
        reset();
    }
    
    T* operator*() {
        return m_value;
    }
    
    T* operator->() {
        return m_value;
    }
    
    T* getValue() {
        return m_value;
    }
    
    const T* getValue() const {
        return m_value;
    }
    
    size_t getSize() const {
        return m_size;
    }
    
private:
    SharedPtr(T* value, size_t size, Deleter deleter, volatile int64_t* counter)
        : m_value(value)
        , m_size(size)
        , m_deleter(deleter)
        , m_counter(counter)
    {}
    
    int64_t increment() {
        return OSIncrementAtomic64(m_counter);
    }
    
    int64_t decrement() {
        return OSDecrementAtomic64(m_counter);
    }
    
    void reset() {
        if (nullptr == m_value) {
            return;
        }
        
        int64_t counter = 2;
        if (nullptr != m_counter) {
            counter = decrement();
        }
        
        if (counter > 1) {
            return;
        }
        
        for (size_t i = 0; i < m_size; ++i) {
            m_deleter(&m_value[i]);
        }
        operator delete[](m_value);
        
        m_counter = nullptr;
        m_value = nullptr;
        m_size = 0;
    }
    
    T* m_value;
    size_t m_size;
    Deleter m_deleter;
    volatile int64_t* m_counter;
};

} // namespace dstd
