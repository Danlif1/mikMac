//
//  SharedPtr.hpp
//  dstd
//
//  Created by Daniel Lifshitz on 04/09/2025.
//
#pragma once

#include "TypeTraites/TypeTraites.hpp"
#include "Checkers.hpp"
#include "Result.hpp"

#include <libkern/OSAtomic.h>

namespace dstd {

template<typename T, typename Deleter = DefaultDeleter<T>>
class SharedPtr {
public:
    SharedPtr(T* value, Deleter deleter, volatile int64_t* counter)
        : m_value(value)
        , m_deleter(deleter)
        , m_counter(counter)
    {}
    
    SharedPtr(const SharedPtr& other)
        : m_value(other.m_value)
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
        m_deleter = other.m_deleter;
        m_counter = other.m_counter;
        
        increment();
        return *this;
    }
    
    SharedPtr(SharedPtr&& other)
        : m_value(other.m_value)
        , m_deleter(move(other.m_deleter))
        , m_counter(other.m_counter)
    {
        other.m_value = nullptr;
        other.m_counter = nullptr;
    }
    
    SharedPtr& operator=(SharedPtr&& other) {
        if (this == &other) {
            return *this;
        }
        
        reset();
        
        m_value = move(other.m_value);
        m_deleter = move(other.m_deleter);
        m_counter = other.m_counter;
        
        other.m_value = nullptr;
        other.m_counter = nullptr;
        return *this;
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
    
    int64_t useCount() const {
        if (nullptr == m_counter) {
            return 0;
        }
        
        return *m_counter;
    }
    
    void replaceValue(SharedPtr<T>&& newValue) {
        if (m_counter == newValue.m_counter) {
            return;
        }
        
        reset();
        
        m_counter = newValue.m_counter;
        m_value = newValue.m_value;
        m_deleter = move(newValue.m_deleter);
        
        newValue.m_counter = nullptr;
        newValue.m_value = nullptr;
    }
    
private:
    int64_t increment() {
        return OSIncrementAtomic64(m_counter);
    }
    
    int64_t decrement() {
        return OSDecrementAtomic64(m_counter);
    }
    
    void reset() {
        if (nullptr == m_value && nullptr == m_counter) {
            return;
        }
        
        int64_t counter = 1;
        if (nullptr != m_counter) {
            counter = decrement();
        }
        
        if (counter > 0) {
            return;
        }
        
        if (nullptr != m_value) {
            m_deleter(m_value);
        }
        
        if (nullptr != m_counter) {
            delete m_counter;
        }
        
        m_counter = nullptr;
        m_value = nullptr;
    }
    
    T* m_value;
    Deleter m_deleter;
    volatile int64_t* m_counter;
};

template<typename T, typename Deleter, typename... Args>
Result<SharedPtr<T, Deleter>> makeSharedWithDeleter(Deleter deleter, Args&&... args) {
    volatile auto counter = new int64_t(1);
    GENERIC_CHECK(nullptr != counter, KERN_NO_SPACE, "Failed to allocate memory for counter");
    
    T* object = new T(forward<Args>(args)...);
    if (nullptr == object) {
        delete counter;
        LOG(LogLevel::LOG_ERROR, "Failed to allocate memory for sharedPtr");
        return Result<void>::makeError(KERN_NO_SPACE);
    }
    
    return Result<SharedPtr<T, Deleter>>::make(SharedPtr(object, deleter, counter));
}

template<typename T, typename... Args>
Result<SharedPtr<T>> makeShared(Args&&... args) {
    return makeSharedWithDeleter<T, DefaultDeleter<T>>(DefaultDeleter<T>(), forward<Args>(args)...);
}

} // namespace dstd
