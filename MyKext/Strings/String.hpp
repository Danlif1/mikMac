//
//  String.hpp
//  MyKext
//
//  Created by Daniel Lifshitz on 06/09/2025.
//
#pragma once

#include "../Result.hpp"

#include "../Pointers/SharedPtr.hpp"


namespace dstd {

class String {
public:
    static Result<String> make(const char* rawString);
    static Result<String> make(const char* rawString, size_t size);
    static Result<String> make(size_t size);
    
    bool operator==(const String& other) const;
    bool operator==(const char* rawOther) const;
    
    Result<String> operator+(const String& other) const;
    
    char* c_str();
    const char* c_str() const;
    
    size_t size() const;
    
private:
    String(SharedPtr<char>&& string);
    
    SharedPtr<char> m_string;
};

const char* strstr(const char* haystack, const char* needle);

} // namespace dstd
