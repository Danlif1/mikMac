//
//  Array.hpp
//  dstd
//
//  Created by Daniel Lifshitz on 03/10/2025.
//
#pragma once

#include <stdint.h>

#include "Checkers.hpp"
#include "Result.hpp"
#include "TypeTraites/TypeTraites.hpp"
#include "Pointers/RawBuffer.hpp"

namespace dstd {

template<typename T, size_t Size>
class Array {
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

    static Result<Array<T, Size>> make() {
        CHECK_RESULT(buffer, allocateBuffer(), "Failed to allocate array buffer");

        T* elements = reinterpret_cast<T*>(buffer.getCharBuffer());
        CHECK_RESULT_NO_VALUE(constructDefaultElements(elements), "Failed to construct array elements");

        return Result<Array<T, Size>>::make(Array(move(buffer)));
    }

    template<typename... Args>
    static Result<Array<T, Size>> make(Args&&... args) {
        CHECK_RESULT(buffer, allocateBuffer(), "Failed to allocate array buffer");

        T* elements = reinterpret_cast<T*>(buffer.getCharBuffer());
        for (size_t index = 0; index < Size; ++index) {
            new (elements + index) T(forward<Args>(args)...);
        }

        return Result<Array<T, Size>>::make(Array(move(buffer)));
    }

    Array(const Array&) = delete;
    Array& operator=(const Array&) = delete;

    Array(Array&& other) noexcept
        : m_buffer(move(other.m_buffer))
        , m_active(other.m_active)
    {
        other.m_active = false;
    }

    Array& operator=(Array&& other) {
        if (this == &other) {
            return *this;
        }

        destroyElements();

        m_buffer = move(other.m_buffer);
        m_active = other.m_active;
        other.m_active = false;

        return *this;
    }

    ~Array() {
        destroyElements();
    }

    T& operator[](size_t index) {
        BREAK_IF_DEBUGGER_PRESENT(index < Size);
        return data()[index];
    }

    const T& operator[](size_t index) const {
        BREAK_IF_DEBUGGER_PRESENT(index < Size);
        return data()[index];
    }

    T& at(size_t index) {
        return (*this)[index];
    }

    const T& at(size_t index) const {
        return (*this)[index];
    }

    T& front() {
        BREAK_IF_DEBUGGER_PRESENT(0 < Size);
        return data()[0];
    }

    const T& front() const {
        BREAK_IF_DEBUGGER_PRESENT(0 < Size);
        return data()[0];
    }

    T& back() {
        BREAK_IF_DEBUGGER_PRESENT(0 < Size);
        return data()[Size - 1];
    }

    const T& back() const {
        BREAK_IF_DEBUGGER_PRESENT(0 < Size);
        return data()[Size - 1];
    }

    T* data() noexcept {
        return reinterpret_cast<T*>(m_buffer.getCharBuffer());
    }

    const T* data() const noexcept {
        return reinterpret_cast<const T*>(m_buffer.getCharBuffer());
    }

    static constexpr size_t size() noexcept {
        return Size;
    }

    static constexpr bool empty() noexcept {
        return 0 == Size;
    }

    Result<void> fill(const T& value) {
        for (size_t index = 0; index < Size; ++index) {
            data()[index] = value;
        }

        return Result<void>::make();
    }

    iterator begin() noexcept {
        return iterator(data());
    }

    iterator end() noexcept {
        return iterator(data() + Size);
    }

    const_iterator begin() const noexcept {
        return const_iterator(data());
    }

    const_iterator end() const noexcept {
        return const_iterator(data() + Size);
    }

    const_iterator cbegin() const noexcept {
        return begin();
    }

    const_iterator cend() const noexcept {
        return end();
    }

private:
    explicit Array(RawBuffer&& buffer)
        : m_buffer(move(buffer))
        , m_active(true)
    {}

    static Result<RawBuffer> allocateBuffer() {
        if (0 == Size) {
            return RawBuffer::make();
        }

        GENERIC_CHECK(Size <= (SIZE_MAX / sizeof(T)), KERN_NO_SPACE, "Array size too large");
        return RawBuffer::make(Size * sizeof(T));
    }

    static Result<void> constructDefaultElement(T* location, true_type) {
        auto elementResult = T::make();
        if (elementResult.hasError()) {
            return Result<void>::makeError(elementResult.error());
        }

        new (location) T(move(elementResult.value()));
        return Result<void>::make();
    }

    static Result<void> constructDefaultElement(T* location, false_type) {
        new (location) T();
        return Result<void>::make();
    }

    static Result<void> constructDefaultElement(T* location) {
        return constructDefaultElement(location, integral_constant<bool, DefaultMakeable<T>::value>{});
    }

    static Result<void> constructDefaultElements(T* elements) {
        for (size_t index = 0; index < Size; ++index) {
            auto constructResult = constructDefaultElement(elements + index);
            if (constructResult.hasError()) {
                destroyConstructedElements(elements, index);
                return constructResult;
            }
        }

        return Result<void>::make();
    }

    static void destroyConstructedElements(T* elements, size_t count) {
        for (size_t index = 0; index < count; ++index) {
            elements[index].~T();
        }
    }

    void destroyElements() {
        if (!m_active) {
            return;
        }

        T* elements = data();
        if (nullptr == elements) {
            return;
        }

        for (size_t index = 0; index < Size; ++index) {
            elements[index].~T();
        }
    }

    RawBuffer m_buffer;
    bool m_active;
};

} // namespace dstd
