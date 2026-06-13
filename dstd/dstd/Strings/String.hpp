//
//  String.hpp
//  dstd
//
//  Created by Daniel Lifshitz on 06/09/2025.
//
#pragma once

#include "Result.hpp"
#include "Pointers/RawBuffer.hpp"
#include "Pointers/SharedPtr.hpp"


namespace dstd {

class String {
public:
    static Result<String> make(const char* rawString);
    static Result<String> make(const char* rawString, size_t length);
    static Result<String> make(size_t capacity);
    
    String(const String& other) = default;
    String(String&& other) = default;
    String& operator=(const String& other) = default;
    String& operator=(String&& other) = default;
    
    bool operator==(const String& other) const;
    bool operator==(const char* rawOther) const;
    
    Result<String> operator+(const String& other) const;
    
    char* c_str();
    const char* c_str() const;
    
    operator char*();
    operator const char*() const;
    
    size_t length() const;
    size_t size() const;
    size_t capacity() const;
    
private:
    struct Data {
        RawBuffer buffer;
        size_t length;
        
        Data(RawBuffer&& buffer, size_t length)
            : buffer(move(buffer))
            , length(length)
        {}
    };
    
    String(SharedPtr<Data>&& data);
    
    static Result<String> makeFromBuffer(RawBuffer&& buffer, size_t length);
    Result<void> ensureUnique();
    
    SharedPtr<Data> m_data;
};

const char* strstr(const char* haystack, const char* needle);

} // namespace dstd
