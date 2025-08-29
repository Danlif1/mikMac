//
//  Optional.hpp
//  MyKext
//
//  Created by Daniel Lifshitz on 22/08/2025.
//
#pragma once

#include <sys/systm.h>
#include <mach/mach_types.h>

#include "TypeTraites/Move.hpp"


namespace dstd {

template<typename T>
class Optional {
public:
    Optional()
        : m_dummy(0)
        , m_isValid(false)
    {}
    
    explicit Optional(const Optional<T>& other) {
        if (!other.m_isValid) {
            destory();
            m_isValid = false;
            return;
        }
        
        m_object = other.m_object;
        m_isValid = true;
    }
    
    explicit Optional(const T& object)
        : m_object(object)
        , m_isValid(true)
    {}
    
    Optional<T>& operator=(const T& object) {
        if (m_isValid && m_object == object) {
            return *this;
        }
        
        if (m_isValid) {
            destory();
        }
        
        m_object = object;
        
        return *this;
    }
    
    Optional<T>& operator=(const Optional<T>& other) {
        if (&other == this) {
            return *this;
        }
        
        if (m_isValid && !other.m_isValid) {
            destory();
        }
        
        m_object = other.m_object;
        
        return *this;
    }
    
    explicit Optional(Optional<T>&& other) {
        if (!other.m_isValid) {
            destory();
            return;
        }
        
        m_object = dstd::move(other.m_object);
        m_isValid = true;
    }
    
    explicit Optional(T&& object)
        : m_object(dstd::move(object))
        , m_isValid(true)
    {}
    
    Optional<T>& operator=(T&& object) {
        if (m_isValid && m_object == object) {
            return *this;
        }
        
        if (m_isValid) {
            destory();
        }
        
        m_object = dstd::move(object);
        
        return *this;
    }
    
    Optional<T>& operator=(Optional<T>&& other) {
        if (m_isValid && &this == other) {
            return *this;
        }
        
        if (!m_isValid && !other.m_isValid) {
            return *this;
        }
        
        m_object = dstd::move(other.m_object);
        
        return *this;
    }
    
    ~Optional() {
        destory();
    }
    
    T* get() {
        if (m_isValid) {
            return &m_object;
        }
        
        return nullptr;
    }
    
    bool isValid() {
        return m_isValid;
    }
    
private:
    void destory() {
        if (m_isValid) {
            m_object.~T();
            m_isValid = false;
            memset(m_dummy, 0, sizeof(T));
        }
    }
    
    union {
        char m_dummy[sizeof(T)];
        T m_object;
    };
    
    bool m_isValid;
};

} // namespace dstd
