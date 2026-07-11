//
//  Set.hpp
//  dstd
//
//  Created by Daniel Lifshitz on 19/06/2026.
//
#pragma once

#include <stdint.h>

#include "Memory/Memory.hpp"
#include "Checkers.hpp"
#include "Result.hpp"
#include "TypeTraites/TypeTraites.hpp"
#include "Pointers/RawBuffer.hpp"

namespace dstd {

template<typename T>
class Set {
public:
    template<typename U>
    struct Iterator {
        using value_type = U;
        using pointer = U*;
        using reference = U&;
        using difference_type = ptrdiff_t;

        pointer m_ptr;

        Iterator() noexcept
            : m_ptr(nullptr)
        {}

        explicit Iterator(pointer ptr) noexcept
            : m_ptr(ptr)
        {}

        reference operator*() const {
            return *m_ptr;
        }

        pointer operator->() const {
            return m_ptr;
        }

        Iterator& operator++() {
            ++m_ptr;
            return *this;
        }

        bool operator==(const Iterator& other) const noexcept {
            return m_ptr == other.m_ptr;
        }

        bool operator!=(const Iterator& other) const noexcept {
            return m_ptr != other.m_ptr;
        }
    };

    using iterator = Iterator<T>;
    using const_iterator = Iterator<const T>;
    using value_type = T;
    using size_type = size_t;

    static Result<Set<T>> make() {
        CHECK_RESULT(buffer, RawBuffer::make(), "Failed to allocate set buffer");
        return Set(move(buffer), 0, 0);
    }

    Set(const Set&) = delete;
    Set& operator=(const Set&) = delete;

    Set(Set&& other) noexcept
        : m_buffer(move(other.m_buffer))
        , m_size(other.m_size)
        , m_capacity(other.m_capacity)
    {
        other.m_size = 0;
        other.m_capacity = 0;
    }

    Set& operator=(Set&& other) {
        if (this == &other) {
            return *this;
        }

        destroyElements(0, m_size);

        m_buffer = move(other.m_buffer);
        m_size = other.m_size;
        m_capacity = other.m_capacity;

        other.m_size = 0;
        other.m_capacity = 0;

        return *this;
    }

    ~Set() {
        destroyElements(0, m_size);
    }

    bool has(const T& value) const {
        for (size_t index = 0; index < m_size; ++index) {
            if (elements()[index] == value) {
                return true;
            }
        }

        return false;
    }

    template<typename U>
    Result<bool> push(U&& value) {
        if (has(value)) {
            return false;
        }

        if (m_size == m_capacity) {
            const size_t newCapacity = (0 == m_capacity) ? 1 : (m_capacity * 2);
            CHECK_RESULT_NO_VALUE(grow(newCapacity), "Failed to grow set");
        }

        constructAt(elements() + m_size, forward<U>(value));
        ++m_size;

        return true;
    }

    Result<void> clear() {
        destroyElements(0, m_size);
        m_size = 0;
        return {};
    }

    size_t size() const noexcept {
        return m_size;
    }

    bool empty() const noexcept {
        return 0 == m_size;
    }

    iterator begin() noexcept {
        return iterator(elements());
    }

    iterator end() noexcept {
        return iterator(elements() + m_size);
    }

    const_iterator begin() const noexcept {
        return const_iterator(elements());
    }

    const_iterator end() const noexcept {
        return const_iterator(elements() + m_size);
    }

    const_iterator cbegin() const noexcept {
        return begin();
    }

    const_iterator cend() const noexcept {
        return end();
    }

private:
    Set(RawBuffer&& buffer, size_t size, size_t capacity)
        : m_buffer(move(buffer))
        , m_size(size)
        , m_capacity(capacity)
    {}

    T* elements() noexcept {
        return reinterpret_cast<T*>(m_buffer.getCharBuffer());
    }

    const T* elements() const noexcept {
        return reinterpret_cast<const T*>(m_buffer.getCharBuffer());
    }

    Result<void> grow(size_t newCapacity) {
        if (newCapacity <= m_capacity) {
            return {};
        }

        GENERIC_CHECK(newCapacity <= (SIZE_MAX / sizeof(T)), KERN_NO_SPACE, "Set capacity too large");

        CHECK_RESULT(newBuffer, RawBuffer::make(newCapacity * sizeof(T)), "Failed to allocate set buffer");
        T* newElements = reinterpret_cast<T*>(newBuffer.getCharBuffer());
        T* oldElements = elements();

        for (size_t index = 0; index < m_size; ++index) {
            constructAt(newElements + index, move(oldElements[index]));
            oldElements[index].~T();
        }

        m_buffer = move(newBuffer);
        m_capacity = newCapacity;

        return {};
    }

    void destroyElements(size_t start, size_t end) {
        T* storedElements = elements();
        if (nullptr == storedElements) {
            return;
        }

        for (size_t index = start; index < end; ++index) {
            storedElements[index].~T();
        }
    }

    RawBuffer m_buffer;
    size_t m_size;
    size_t m_capacity;
};

} // namespace dstd
