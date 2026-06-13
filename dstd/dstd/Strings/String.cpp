//
//  String.cpp
//  dstd
//
//  Created by Daniel Lifshitz on 06/09/2025.
//
#include "String.hpp"

#include "Checkers.hpp"

#include <string.h>


namespace dstd {

Result<String> String::make(const char* rawString) {
    GENERIC_CHECK(nullptr != rawString, KERN_INVALID_ARGUMENT, "Null string pointer");
    return make(rawString, strlen(rawString));
}

Result<String> String::make(const char* rawString, size_t length) {
    GENERIC_CHECK(nullptr != rawString, KERN_INVALID_ARGUMENT, "Null string pointer");
    GENERIC_CHECK(length < length + 1, KERN_NO_SPACE, "Size too big");
    
    CHECK_RESULT(buffer, RawBuffer::make(length + 1), "Failed to allocate buffer");
    
    if (length > 0) {
        memcpy(buffer.getCharBuffer(), rawString, length);
    }
    buffer.getCharBuffer()[length] = '\0';
    
    return makeFromBuffer(move(buffer), length);
}

Result<String> String::make(size_t capacity) {
    GENERIC_CHECK(capacity < capacity + 1, KERN_NO_SPACE, "Size too big");
    
    CHECK_RESULT(buffer, RawBuffer::make(capacity + 1), "Failed to allocate buffer");
    memset(buffer.getCharBuffer(), '\0', capacity + 1);
    
    return makeFromBuffer(move(buffer), 0);
}

bool String::operator==(const String& other) const {
    const size_t thisLength = length();
    const size_t otherLength = other.length();
    
    if (thisLength != otherLength) {
        return false;
    }
    
    if (0 == thisLength) {
        return true;
    }
    
    return 0 == strncmp(c_str(), other.c_str(), thisLength);
}

bool String::operator==(const char* rawOther) const {
    if (nullptr == rawOther) {
        return false;
    }
    
    const size_t thisLength = length();
    if (0 != strncmp(c_str(), rawOther, thisLength)) {
        return false;
    }
    
    return '\0' == rawOther[thisLength];
}

Result<String> String::operator+(const String& other) const {
    const size_t newLength = length() + other.length();
    GENERIC_CHECK(newLength >= length(), KERN_NO_SPACE, "Total size too large");
    GENERIC_CHECK(newLength >= other.length(), KERN_NO_SPACE, "Total size too large");
    
    CHECK_RESULT(buffer, RawBuffer::make(newLength + 1), "Failed to allocate buffer");
    
    if (length() > 0) {
        memcpy(buffer.getCharBuffer(), c_str(), length());
    }
    if (other.length() > 0) {
        memcpy(buffer.getCharBuffer() + length(), other.c_str(), other.length());
    }
    buffer.getCharBuffer()[newLength] = '\0';
    
    return makeFromBuffer(move(buffer), newLength);
}

char* String::c_str() {
    auto ensureResult = ensureUnique();
    BREAK_IF_DEBUGGER_PRESENT(!ensureResult.hasError());
    return m_data.getValue()->buffer.getCharBuffer();
}

const char* String::c_str() const {
    return m_data.getValue()->buffer.getCharBuffer();
}

String::operator char*() {
    return c_str();
}

String::operator const char*() const {
    return c_str();
}

size_t String::length() const {
    const char* string = m_data.getValue()->buffer.getCharBuffer();
    if (nullptr == string) {
        return 0;
    }
    
    return strlen(string);
}

size_t String::size() const {
    return length() + 1;
}

size_t String::capacity() const {
    const size_t bufferSize = m_data.getValue()->buffer.size();
    if (0 == bufferSize) {
        return 0;
    }
    
    return bufferSize - 1;
}

String::String(SharedPtr<Data>&& data)
    : m_data(move(data))
{}

Result<String> String::makeFromBuffer(RawBuffer&& buffer, size_t length) {
    CHECK_RESULT(data, makeShared<Data>(move(buffer), length), "Failed to create shared string data");
    return Result<String>::make(String(move(data)));
}

Result<void> String::ensureUnique() {
    if (nullptr == m_data.getValue()) {
        return Result<void>::makeError(KERN_FAILURE);
    }
    
    if (m_data.useCount() <= 1) {
        return Result<void>::make();
    }
    
    CHECK_RESULT(bufferCopy, RawBuffer::copy(m_data.getValue()->buffer), "Failed to copy string buffer for COW");
    const size_t length = strlen(m_data.getValue()->buffer.getCharBuffer());
    CHECK_RESULT(data, makeShared<Data>(move(bufferCopy), length), "Failed to create detached string data");
    m_data = move(data);
    
    return Result<void>::make();
}

const char* strstr(const char* haystack, const char* needle) {
    const size_t needleLength = strlen(needle);
    while ('\0' != *haystack) {
        if (!memcmp(haystack, needle, needleLength)) {
            return haystack;
        }
        
        haystack++;
    }
    
    return nullptr;
}

} // namespace dstd
