//
//  RawBuffer.hpp
//  dstd
//
//  Created by Daniel Lifshitz on 24/10/2025.
//
#pragma once

#include "Result.hpp"
#include "UniquePtr.hpp"

namespace dstd {

class RawBufferView {
public:
    RawBufferView(const char* buffer, size_t size);
    
    const char* getCharBuffer() const;
    const void* get() const;
    size_t size() const;

private:
    const char* m_buffer;
    size_t m_size;
};

class RawBuffer {
public:
    static Result<RawBuffer> make();
    static Result<RawBuffer> make(size_t size);
    static Result<RawBuffer> copy(const RawBuffer& other);
    
    Result<void> resize(size_t newSize);
    Result<void> resizeIfNeeded(size_t neededSize);
    
    char* getCharBuffer();
    const char* getCharBuffer() const;
    
    void* get();
    const void* get() const;
    
    size_t size() const;
    
    void clear();
    
    Result<void> append(RawBufferView&& buffer);
    Result<void> append(RawBufferView& buffer);
    Result<void> append(RawBuffer&& buffer);

private:
    RawBuffer(UniquePtr<char, ArrayDeleter<char>>&& buffer, size_t size, size_t used = 0);
    
    UniquePtr<char, ArrayDeleter<char>> m_buffer;
    size_t m_size;
    size_t m_used;
};

} // namespace dstd
