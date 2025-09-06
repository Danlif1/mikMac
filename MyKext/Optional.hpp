//
//  Optional.hpp
//  MyKext
//
//  Created by Daniel Lifshitz on 22/08/2025.
//
#pragma once

#include <sys/systm.h>
#include <mach/mach_types.h>

#include "Memory/Memory.hpp"

#include "TypeTraites/Move.hpp"


namespace dstd {

template<typename T>
class Optional {
public:
    /**
        Default constructor.
     */
    Optional()
        : m_dummy(0)
        , m_hasValue(false)
    {}
    
    /**
        Copy constructors.
     */
    Optional(const Optional<T>& other) {
        if (!other.m_hasValue) {
            m_hasValue = false;
            return;
        }
        
        new (&m_object) T(other.m_object);
        m_hasValue = true;
    }
    explicit Optional(const T& object)
        : m_object(object)
        , m_hasValue(true)
    {}
    
    /**
        Copy assignment operators.
     */
    Optional<T>& operator=(const T& object) {
        if (m_hasValue && m_object == object) {
            return *this;
        }
        
        if (m_hasValue) {
            reset();
        }
        
        new (&m_object) T(object);
        m_hasValue = true;
        
        return *this;
    }
    Optional<T>& operator=(const Optional<T>& other) {
        if (this == &other) {
            return *this;
        }
        
        reset();
        
        if (!other.m_hasValue) {
            return *this;
        }
        
        new (&m_object) T(other.m_object);
        m_hasValue = true;
        
        return *this;
    }
    
    /**
        Move constructors.
     */
    Optional(Optional<T>&& other) {
        if (!other.m_hasValue) {
            m_hasValue = false;
            return;
        }
        
        new (&m_object) T(dstd::move(other.m_object));
        m_hasValue = true;
    }
    explicit Optional(T&& object)
        : m_object(dstd::move(object))
        , m_hasValue(true)
    {}
    
    /**
        Move assignment operators.
     */
    Optional<T>& operator=(T&& object) {
        if (m_hasValue && m_object == object) {
            return *this;
        }
        
        reset();
        
        new (&m_object) T(dstd::move(object));
        m_hasValue = true;
        
        return *this;
    }
    Optional<T>& operator=(Optional<T>&& other) {
        if (this == &other) {
            return *this;
        }
        
        reset();
        
        if (!other.m_hasValue) {
            return *this;
        }
                
        new (&m_object) T(dstd::move(other.m_object));
        m_hasValue = true;
        
        return *this;
    }
    
    /**
        Destructor.
     */
    ~Optional() {
        reset();
    }
    
    /**
        Equality operators.
     */
    bool operator==(const Optional<T>& other) const{
        // Both are invalid.
        if (!m_hasValue && !other.m_hasValue) {
            return true;
        }
        
        // One of them is valid and the other is not.
        if (!m_hasValue || !other.m_hasValue) {
            return false;
        }
        
        return m_object == other.m_object;
    }
    bool operator==(const T& object) const {
        if (!m_hasValue) {
            return false;
        }
        
        return m_object == object;
    }
    
    /**
        We trust the user to use it only when it's valid.
     */
    T& value() {
        return m_object;
    }
    
    /**
        We trust the user to use it only when it's valid.
     */
    const T& value() const {
        return m_object;
    }
    
    /**
        @returns If there is currently a value or not in the object.
     */
    bool hasValue() const {
        return m_hasValue;
    }
    
    /**
        Calls the destructor if the object is valid.
     */
    void reset() {
        if (m_hasValue) {
            m_object.~T();
            m_hasValue = false;
        }
    }
    
private:
    union {
        char m_dummy[sizeof(T)];
        T m_object;
    };
    
    bool m_hasValue;
};

} // namespace dstd
