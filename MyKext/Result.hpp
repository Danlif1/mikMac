//
//  Result.hpp
//  MyKext
//
//  Created by Daniel Lifshitz on 29/08/2025.
//
#pragma once

#include <mach/mach_types.h>

#include "Optional.hpp"

namespace dstd {

template<typename T>
class Result;

template<>
class Result<void> {
public:
    static Result<void> make(kern_return_t status = KERN_SUCCESS) {
        return makeError(status);
    }
    
    static Result<void> makeError(kern_return_t status) {
        return Result(status);
    }
    
    bool hasValue() const {
        return KERN_SUCCESS == m_error;
    }
    
    kern_return_t value() const {
        // in <void> the value is also the error so we return it here too.
        return m_error;
    }
    
    bool hasError() const {
        return !hasValue();
    }
    
    kern_return_t error() const {
        return m_error;
    }
    
    operator kern_return_t() {
        return m_error;
    }
    
    Result(kern_return_t status = KERN_SUCCESS)
        : m_error(status)
    {}
    
private:
    kern_return_t m_error;
};

template<typename T>
class Result {
public:
    static Result make(T&& value) {
        return Result(move(value));
    }
    
    static Result makeError(kern_return_t status) {
        return Result<void>(status);
    }
    
    bool hasValue() const {
        return KERN_SUCCESS == m_error;
    }
    
    /**
        We trust the user to use it only when it's valid.
     */
    T& value() {
        return m_value.value();
    }
    
    /**
        We trust the user to use it only when it's valid.
     */
    const T& value() const {
        return m_value.value();
    }
    
    bool hasError() const {
        return !hasValue();
    }
    
    kern_return_t error() const {
        return m_error;
    }
    
    operator kern_return_t() const {
        return m_error;
    }
    
    Result()
        : m_error(KERN_SUCCESS)
    {}
    
    Result(const Result<void>& other)
        : m_error(other.error())
    {}
    
    Result(Result<void>&& other)
        : m_error(other.error())
    {}
    
private:
    explicit Result(T&& value)
        : m_value(move(value))
        , m_error(KERN_SUCCESS)
    {}
    
    Optional<T> m_value;
    kern_return_t m_error;
};

} // namespace dstd
