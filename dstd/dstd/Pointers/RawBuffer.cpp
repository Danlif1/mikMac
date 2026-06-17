//
//  RawBuffer.cpp
//  Escalator
//
//  Created by Daniel Lifshitz on 30/11/2025.
//
#include "RawBuffer.hpp"

#include "Basics.hpp"
#include "Checkers.hpp"

#include <string.h>


namespace dstd {

namespace {

Result<UniquePtr<char, ArrayDeleter<char>>> allocateBuffer(size_t size) {
    if (0 == size) {
        return UniquePtr<char, ArrayDeleter<char>>(nullptr, ArrayDeleter<char>());
    }
    
    char* buffer = new char[size];
    GENERIC_CHECK(nullptr != buffer, KERN_NO_SPACE, "Failed to allocate buffer");
    
    return UniquePtr<char, ArrayDeleter<char>>(buffer, ArrayDeleter<char>());
}

} // namespace

RawBufferView::RawBufferView(const char* buffer, size_t size)
    : m_buffer(buffer)
    , m_size(size)
{}

const char* RawBufferView::getCharBuffer() const {
    return m_buffer;
}

const void* RawBufferView::get() const {
    return m_buffer;
}

size_t RawBufferView::size() const {
    return m_size;
}

Result<RawBuffer> RawBuffer::make() {
    return make(0);
}

Result<RawBuffer> RawBuffer::make(size_t size) {
    CHECK_RESULT(buffer, allocateBuffer(size), "Failed to allocate buffer");
    return RawBuffer(move(buffer), size);
}

Result<RawBuffer> RawBuffer::copy(const RawBuffer& other) {
    CHECK_RESULT(buffer, allocateBuffer(other.size()), "Failed to create buffer");
    
    if (other.size() > 0 && nullptr != other.get()) {
        memcpy(buffer.getValue(), other.get(), other.size());
    }
    
    return RawBuffer(move(buffer), other.size(), other.m_used);
}

Result<void> RawBuffer::resize(size_t newSize) {
    CHECK_RESULT(newBuffer, allocateBuffer(newSize), "Failed to allocate new buffer");
    
    const size_t copySize = min(m_size, newSize);
    if (copySize > 0 && nullptr != getCharBuffer() && nullptr != newBuffer.getValue()) {
        memcpy(newBuffer.getValue(), getCharBuffer(), copySize);
    }
    
    m_buffer = move(newBuffer);
    m_size = newSize;
    m_used = min(m_used, newSize);
    
    return {};
}

Result<void> RawBuffer::resizeIfNeeded(size_t neededSize) {
    if (neededSize <= m_size) {
        if (neededSize >= m_size / 2) {
            return {};
        }
        
        CHECK_RESULT_NO_VALUE(resize(neededSize), "Failed to resize the buffer");
        m_used = min(m_used, neededSize);
        return {};
    }
    
    CHECK_RESULT_NO_VALUE(resize(max(neededSize, 2 * m_size)), "Failed to resize the buffer");
    
    return {};
}

char* RawBuffer::getCharBuffer() {
    return m_buffer.getValue();
}

const char* RawBuffer::getCharBuffer() const {
    return m_buffer.getValue();
}

void* RawBuffer::get() {
    return m_buffer.getValue();
}

const void* RawBuffer::get() const {
    return m_buffer.getValue();
}

size_t RawBuffer::size() const {
    return m_size;
}

void RawBuffer::clear() {
    if (0 == m_size || nullptr == m_buffer.getValue()) {
        return;
    }
    
    memset(m_buffer.getValue(), 0, m_size);
}

Result<void> RawBuffer::append(RawBufferView&& buffer) {
    const size_t addedSize = buffer.size();
    GENERIC_CHECK(m_used + addedSize >= addedSize, KERN_NO_SPACE, "Trying to allocate more then size_t bytes");
    GENERIC_CHECK(m_used + addedSize >= m_used, KERN_NO_SPACE, "Trying to allocate more then size_t bytes");
    CHECK_RESULT_NO_VALUE(resizeIfNeeded(m_used + addedSize), "Failed resizing the buffer");
    GENERIC_CHECK(m_used + addedSize <= m_size, KERN_FAILURE, "WTF we should not get here");
    
    memcpy(getCharBuffer() + m_used, buffer.get(), addedSize);
    m_used += addedSize;
    
    return {};
}

Result<void> RawBuffer::append(RawBufferView& buffer) {
    return append(move(buffer));
}

Result<void> RawBuffer::append(RawBuffer&& buffer) {
    const size_t bytesToAppend = buffer.m_used > 0 ? buffer.m_used : buffer.size();
    return append(RawBufferView(buffer.getCharBuffer(), bytesToAppend));
}

RawBuffer::RawBuffer(UniquePtr<char, ArrayDeleter<char>>&& buffer, size_t size, size_t used)
    : m_buffer(move(buffer))
    , m_size(size)
    , m_used(used)
{}

} // namespace dstd
