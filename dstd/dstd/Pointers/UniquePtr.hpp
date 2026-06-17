//
//  UniquePtr.hpp
//  dstd
//
//  Created by Daniel Lifshitz on 22/08/2025.
//
#pragma once

#include "TypeTraites/TypeTraites.hpp"
#include "Basics.hpp"
#include "Result.hpp"


namespace dstd {

template<typename T, typename Deleter = DefaultDeleter<T>>
class UniquePtr {
public:
    template<typename... Args>
    static Result<UniquePtr<T, Deleter>> make(Args&&... args) {
        T* object = new T(forward<Args>(args)...);
        GENERIC_CHECK_NO_LOG(nullptr != object, KERN_NO_SPACE);

        return Result<UniquePtr<T, Deleter>>(UniquePtr<T, Deleter>(object, Deleter()));
    }
    
    UniquePtr(T* value, Deleter deleter = Deleter())
        : m_value(value)
        , m_deleter(deleter)
    {}
    
    UniquePtr(UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;
    
    UniquePtr(UniquePtr&& other)
        : m_value(other.m_value)
        , m_deleter(other.m_deleter)
    {
        other.m_value = nullptr;
    }
    
    UniquePtr& operator=(UniquePtr&& other) {
        if (this == &other) {
            return *this;
        }
        
        reset();
        
        m_value = other.m_value;
        m_deleter = other.m_deleter;
        
        other.m_value = nullptr;
        
        return *this;
    }
    
    ~UniquePtr() {
        reset();
    }
    
    bool operator==(const UniquePtr<T, Deleter>& other) const {
        return m_value == other.m_value;
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
    
    T* release() {
        auto value = m_value;
        m_value = nullptr;
        
        return value;
    }
    
private:
    void reset() {
        if (nullptr == m_value) {
            return;
        }

        m_deleter(m_value);
        
        m_value = nullptr;
    }
    
    T* m_value;
    Deleter m_deleter;
};


template<typename T, typename Deleter, typename... Args>
Result<UniquePtr<T, Deleter>> makeUniqueWithDeleter(Deleter deleter, Args&&... args) {
    T* object = new T(forward<Args>(args)...);
    GENERIC_CHECK_NO_LOG(nullptr != object, KERN_NO_SPACE);

    return Result<UniquePtr<T, Deleter>>(UniquePtr<T, Deleter>(object, deleter));
}

template<typename T, typename... Args>
Result<UniquePtr<T>> makeUnique(Args&&... args) {
    return makeUniqueWithDeleter<T, DefaultDeleter<T>, Args...>(DefaultDeleter<T>(), forward<Args>(args)...);
}

} // namespace dstd
