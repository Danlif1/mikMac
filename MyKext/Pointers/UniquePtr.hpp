//
//  UniquePtr.hpp
//  MyKext
//
//  Created by Daniel Lifshitz on 22/08/2025.
//
#pragma once

#include "../Checkers.hpp"
#include "../Logger.hpp"
#include "../Result.hpp"


namespace dstd {

template<typename T, typename Deleter = DefaultDeleter<T>>
class UniquePtr {
public:
    template<typename... Args>
    static Result<UniquePtr<T, Deleter>> make(Args&&... args) {
        return makeArray(1, move(args)...);
    }
    
    template<typename... Args>
    static Result<UniquePtr<T, Deleter>> makeArray(const size_t size, Args&&... args) {
        return makeArrayWithDeleter(Deleter(), size, move(args)...);
    }
    
    template<typename... Args>
    static Result<UniquePtr<T, Deleter>> makeArrayWithDeleter(Deleter deleter, const size_t size, Args&&... args) {
        void* raw = operator new[](size * sizeof(T));
        GENERIC_CHECK(nullptr != raw, KERN_NO_SPACE, "Failed to allocate memory");
        
        auto array = static_cast<T*>(raw);
        
        for (size_t i = 0; i < size; i++) {
            new (&array[i]) T(move(args)...);
        }
        
        UniquePtr result(array, size, deleter);
        return Result<UniquePtr<T, Deleter>>::make(move(result));
    }
    
    UniquePtr(UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;
    
    UniquePtr(UniquePtr&& other)
        : m_value(other.m_value)
        , m_size(other.m_size)
        , m_deleter(other.m_deleter)
    {
        other.m_value = nullptr;
        other.m_size = 0;
    }
    
    UniquePtr& operator=(UniquePtr&& other) {
        if (this == &other) {
            return *this;
        }
        
        reset();
        
        m_value = other.m_value;
        m_size = other.m_size;
        m_deleter = other.m_deleter;
        
        other.m_value = nullptr;
        other.m_size = 0;
    }
    
    ~UniquePtr() {
        reset();
    }
    
    bool operator==(const UniquePtr<T, Deleter>& other) const {
        return m_value == other.m_value && m_size == other.m_size;
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
    UniquePtr(T* value, size_t size, Deleter deleter)
        : m_value(value)
        , m_size(size)
        , m_deleter(deleter)
    {}
    
    void reset() {
        if (nullptr == m_value) {
            return;
        }

        for (size_t i = 0; i < m_size; ++i) {
            m_deleter(&m_value[i]);
        }
        operator delete[](m_value);
        
        m_value = nullptr;
        m_size = 0;
    }
    
    T* m_value;
    size_t m_size;
    Deleter m_deleter;
};

} // namespace dstd
