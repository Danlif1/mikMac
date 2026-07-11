//
//  Optional.hpp
//  dstd
//
//  Created by Daniel Lifshitz on 22/08/2025.
//
#pragma once

#include "Basics.hpp"

#include "Memory/Memory.hpp"
#include "TypeTraites/Move.hpp"

#include <sys/systm.h>
#include <mach/mach_types.h>


namespace dstd {

template<typename T>
class Optional {
public:
    Optional()
        : m_dummy(0)
        , m_hasValue(false)
    {}
    
    Optional(const Optional<T>& other) {
        if (!other.m_hasValue) {
            m_hasValue = false;
            return;
        }
        
        constructAt(&m_object, other.m_object);
        m_hasValue = true;
    }
    
    explicit Optional(const T& object)
        : m_object(object)
        , m_hasValue(true)
    {}
    
    Optional<T>& operator=(const T& object) {
        if (m_hasValue && m_object == object) {
            return *this;
        }
        
        if (m_hasValue) {
            reset();
        }
        
        constructAt(&m_object, object);
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
        
        constructAt(&m_object, other.m_object);
        m_hasValue = true;
        
        return *this;
    }
    
    Optional(Optional<T>&& other) {
        if (!other.m_hasValue) {
            m_hasValue = false;
            return;
        }
        
        constructAt(&m_object, dstd::move(other.m_object));
        m_hasValue = true;
        other.m_hasValue = false;
    }
    
    explicit Optional(T&& object)
        : m_object(dstd::move(object))
        , m_hasValue(true)
    {}
    
    Optional<T>& operator=(T&& object) {
        if (m_hasValue && m_object == object) {
            return *this;
        }
        
        reset();
        
        constructAt(&m_object, dstd::move(object));
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
                
        constructAt(&m_object, dstd::move(other.m_object));
        m_hasValue = true;
        other.m_hasValue = false;
        
        return *this;
    }

    template<typename... Args>
    void emplace(Args&&... args) {
        reset();
        constructAt(&m_object, forward<Args>(args)...);
        m_hasValue = true;
    }
    
    ~Optional() {
        reset();
    }
    
    bool operator==(const Optional<T>& other) const {
        if (!m_hasValue && !other.m_hasValue) {
            return true;
        }
        
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
    
    T& value() {
        BREAK_IF_DEBUGGER_PRESENT(hasValue());
        return m_object;
    }
    
    const T& value() const {
        BREAK_IF_DEBUGGER_PRESENT(hasValue());
        return m_object;
    }
    
    bool hasValue() const {
        return m_hasValue;
    }
    
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
