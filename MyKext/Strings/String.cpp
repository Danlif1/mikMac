//
//  String.cpp
//  MyKext
//
//  Created by Daniel Lifshitz on 06/09/2025.
//
#include "String.hpp"

#include <mach/mach_types.h>
#include <string.h>


namespace dstd {

Result<String> String::make(const char* rawString) {
    return make(rawString, strlen(rawString));
}

Result<String> String::make(const char* rawString, size_t size) {
    GENERIC_CHECK(size < size + 1, KERN_NO_SPACE);
    CHECK_RESULT(sharedString, SharedPtr<char>::makeArray(size + 1));
    
    strncpy(sharedString.getValue(), rawString, sharedString.getSize());
    // Ensure null termination
    sharedString.getValue()[sharedString.getSize() - 1] = '\0';
    
    return Result<String>::make(move(String(move(sharedString))));
}

bool String::operator==(const String& other) const {
    return other == c_str();
}

bool String::operator==(const char* rawOther) const{
    return 0 == strcmp(c_str(), rawOther);
}

Result<String> String::operator+(const String& other) const {
    // Check that we don't overflow.
    size_t totalSize = size() + other.size();
    GENERIC_CHECK(totalSize > other.size(), KERN_NO_SPACE);
    GENERIC_CHECK(totalSize > size(), KERN_NO_SPACE);
    
    CHECK_RESULT(sharedString, SharedPtr<char>::makeArray(totalSize));
    // copies size() bytes, currently hold the entire data including null terminator.
    strncpy(sharedString.getValue(), c_str(), size());
    // copies other.size() more bytes but overrides the null terminator, total size() + other.size() - 1 was written.
    strncat(sharedString.getValue(), other.c_str(), totalSize);
    
    return Result<String>::make(move(String(move(sharedString))));
}

char* String::c_str() {
    return m_string.getValue();
}

const char* String::c_str() const {
    return m_string.getValue();
}

size_t String::size() const {
    return m_string.getSize();
}

String::String(SharedPtr<char>&& sharedString)
    : m_string(move(sharedString))
{}

const char* strstr(const char* haystack, const char* needle) {
    size_t n = strlen(needle);
    while('\0' != *haystack) {
        if(!memcmp(haystack,needle,n)) {
            return haystack;
        }
        
        haystack++;
    }
    
    return nullptr;
}

}
