//
//  Vector.hpp
//  dstd
//
//  Created by Daniel Lifshitz on 01/10/2025.
//
#pragma once

#include <stdint.h>

#include "Checkers.hpp"
#include "Result.hpp"
#include "TypeTraites/TypeTraites.hpp"
#include "Pointers/RawBuffer.hpp"

namespace dstd {

template<typename T>
class Vector {
public:
    template<typename U>
    struct RandomAccessIterator {
        using value_type = U;
        using pointer = U*;
        using reference = U&;
        using difference_type = ptrdiff_t;

        pointer m_ptr;

        RandomAccessIterator() noexcept
            : m_ptr(nullptr)
        {}

        explicit RandomAccessIterator(pointer ptr) noexcept
            : m_ptr(ptr)
        {}

        reference operator*() const {
            return *m_ptr;
        }

        pointer operator->() const {
            return m_ptr;
        }

        RandomAccessIterator& operator++() {
            ++m_ptr;
            return *this;
        }

        RandomAccessIterator& operator--() {
            --m_ptr;
            return *this;
        }

        RandomAccessIterator operator+(difference_type offset) const {
            return RandomAccessIterator(m_ptr + offset);
        }

        RandomAccessIterator operator-(difference_type offset) const {
            return RandomAccessIterator(m_ptr - offset);
        }

        difference_type operator-(const RandomAccessIterator& other) const {
            return m_ptr - other.m_ptr;
        }

        RandomAccessIterator& operator+=(difference_type offset) {
            m_ptr += offset;
            return *this;
        }

        RandomAccessIterator& operator-=(difference_type offset) {
            m_ptr -= offset;
            return *this;
        }

        reference operator[](difference_type index) const {
            return m_ptr[index];
        }

        bool operator==(const RandomAccessIterator& other) const noexcept {
            return m_ptr == other.m_ptr;
        }

        bool operator!=(const RandomAccessIterator& other) const noexcept {
            return m_ptr != other.m_ptr;
        }

        bool operator<(const RandomAccessIterator& other) const noexcept {
            return m_ptr < other.m_ptr;
        }

        bool operator<=(const RandomAccessIterator& other) const noexcept {
            return m_ptr <= other.m_ptr;
        }

        bool operator>(const RandomAccessIterator& other) const noexcept {
            return m_ptr > other.m_ptr;
        }

        bool operator>=(const RandomAccessIterator& other) const noexcept {
            return m_ptr >= other.m_ptr;
        }
    };

    using iterator = RandomAccessIterator<T>;
    using const_iterator = RandomAccessIterator<const T>;
    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    static Result<Vector<T>> make() {
        CHECK_RESULT(buffer, RawBuffer::make(), "Failed to allocate vector buffer");
        return Result<Vector<T>>::make(Vector(move(buffer), 0, 0));
    }

    static Result<Vector<T>> make(size_t capacity) {
        if (0 == capacity) {
            return make();
        }

        GENERIC_CHECK(capacity <= (SIZE_MAX / sizeof(T)), KERN_NO_SPACE, "Vector capacity too large");

        CHECK_RESULT(buffer, RawBuffer::make(capacity * sizeof(T)), "Failed to allocate vector buffer");
        return Result<Vector<T>>::make(Vector(move(buffer), 0, capacity));
    }

    Vector(const Vector&) = delete;
    Vector& operator=(const Vector&) = delete;

    Vector(Vector&& other) noexcept
        : m_buffer(move(other.m_buffer))
        , m_size(other.m_size)
        , m_capacity(other.m_capacity)
    {
        other.m_size = 0;
        other.m_capacity = 0;
    }

    Vector& operator=(Vector&& other) {
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

    ~Vector() {
        destroyElements(0, m_size);
    }

    T& operator[](size_t index) {
        BREAK_IF_DEBUGGER_PRESENT(index < m_size);
        return data()[index];
    }

    const T& operator[](size_t index) const {
        BREAK_IF_DEBUGGER_PRESENT(index < m_size);
        return data()[index];
    }

    T& at(size_t index) {
        return (*this)[index];
    }

    const T& at(size_t index) const {
        return (*this)[index];
    }

    T& front() {
        BREAK_IF_DEBUGGER_PRESENT(0 < m_size);
        return data()[0];
    }

    const T& front() const {
        BREAK_IF_DEBUGGER_PRESENT(0 < m_size);
        return data()[0];
    }

    T& back() {
        BREAK_IF_DEBUGGER_PRESENT(0 < m_size);
        return data()[m_size - 1];
    }

    const T& back() const {
        BREAK_IF_DEBUGGER_PRESENT(0 < m_size);
        return data()[m_size - 1];
    }

    T* data() noexcept {
        return reinterpret_cast<T*>(m_buffer.getCharBuffer());
    }

    const T* data() const noexcept {
        return reinterpret_cast<const T*>(m_buffer.getCharBuffer());
    }

    size_t size() const noexcept {
        return m_size;
    }

    size_t capacity() const noexcept {
        return m_capacity;
    }

    bool empty() const noexcept {
        return 0 == m_size;
    }

    iterator begin() noexcept {
        return iterator(data());
    }

    iterator end() noexcept {
        return iterator(data() + m_size);
    }

    const_iterator begin() const noexcept {
        return const_iterator(data());
    }

    const_iterator end() const noexcept {
        return const_iterator(data() + m_size);
    }

    const_iterator cbegin() const noexcept {
        return begin();
    }

    const_iterator cend() const noexcept {
        return end();
    }

    template<typename... Args>
    Result<void> push_back(Args&&... args) {
        if (m_size == m_capacity) {
            const size_t newCapacity = (0 == m_capacity) ? 1 : (m_capacity * 2);
            CHECK_RESULT_NO_VALUE(reserve(newCapacity), "Failed to reserve vector capacity");
        }

        new (data() + m_size) T(forward<Args>(args)...);
        ++m_size;

        return Result<void>::make();
    }

    Result<void> pop_back() {
        GENERIC_CHECK(0 != m_size, KERN_INSUFFICIENT_BUFFER_SIZE, "Vector is already empty");

        data()[m_size - 1].~T();
        --m_size;

        return Result<void>::make();
    }

    Result<void> clear() {
        destroyElements(0, m_size);
        m_size = 0;
        return Result<void>::make();
    }

    Result<void> reserve(size_t newCapacity) {
        if (newCapacity <= m_capacity) {
            return Result<void>::make();
        }

        GENERIC_CHECK(newCapacity <= (SIZE_MAX / sizeof(T)), KERN_NO_SPACE, "Vector capacity too large");

        CHECK_RESULT(newBuffer, RawBuffer::make(newCapacity * sizeof(T)), "Failed to allocate vector buffer");
        T* newData = reinterpret_cast<T*>(newBuffer.getCharBuffer());
        T* oldData = data();

        for (size_t index = 0; index < m_size; ++index) {
            new (newData + index) T(move(oldData[index]));
            oldData[index].~T();
        }

        m_buffer = move(newBuffer);
        m_capacity = newCapacity;

        return Result<void>::make();
    }

    Result<void> resize(size_t count) {
        if (count < m_size) {
            destroyElements(count, m_size);
            m_size = count;
            return Result<void>::make();
        }

        if (count == m_size) {
            return Result<void>::make();
        }

        if (count > m_capacity) {
            CHECK_RESULT_NO_VALUE(reserve(count), "Failed to reserve vector memory");
        }

        for (size_t index = m_size; index < count; ++index) {
            new (data() + index) T();
        }

        m_size = count;
        return Result<void>::make();
    }

private:
    Vector(RawBuffer&& buffer, size_t size, size_t capacity)
        : m_buffer(move(buffer))
        , m_size(size)
        , m_capacity(capacity)
    {}

    void destroyElements(size_t start, size_t end) {
        T* elements = data();
        if (nullptr == elements) {
            return;
        }

        for (size_t index = start; index < end; ++index) {
            elements[index].~T();
        }
    }

    RawBuffer m_buffer;
    size_t m_size;
    size_t m_capacity;
};

} // namespace dstd
