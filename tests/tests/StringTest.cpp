//
//  StringTest.cpp
//  tests
//
#include "Framework/TestFramework.hpp"

#include "Strings/String.hpp"

#include <string.h>


using namespace dstd;
using namespace tests;

dstd::Result<void> tests::testString() {
    static_assert(is_same<Result<String>, decltype(String::make("abc"))>::value, "String::make interface changed");

    CHECK_RESULT(stringValue, String::make("hello"), "String::make(const char*) failed");
    TEST_ASSERT(stringValue == "hello", "String should compare equal to C string");
    TEST_ASSERT(stringValue == stringValue, "String should compare equal to itself");
    TEST_ASSERT_EQ(5, stringValue.length(), "String length should exclude null terminator");
    TEST_ASSERT_EQ(6, stringValue.size(), "String size should include null terminator");
    TEST_ASSERT_EQ(5, stringValue.capacity(), "String capacity should exclude null terminator");

    CHECK_RESULT(prefixValue, String::make("hello", 3), "String::make with length failed");
    TEST_ASSERT(prefixValue == "hel", "String constructed with length should truncate correctly");
    TEST_ASSERT_EQ(3, prefixValue.length(), "String constructed with length should store exact length");

    CHECK_RESULT(reservedValue, String::make(static_cast<size_t>(8)), "String::make(capacity) failed");
    TEST_ASSERT_EQ(0, reservedValue.length(), "reserved String should start empty");
    TEST_ASSERT(reservedValue.capacity() >= 8, "reserved String should honor requested capacity");

    String copiedValue = stringValue;
    char* mutableCopy = copiedValue.c_str();
    mutableCopy[0] = 'j';
    TEST_ASSERT(stringValue == "hello", "copy-on-write String should not mutate shared original");
    TEST_ASSERT(copiedValue == "jello", "mutable String copy should reflect local mutation");

    CHECK_RESULT(concatenatedValue, stringValue + prefixValue, "String concatenation failed");
    TEST_ASSERT(concatenatedValue == "hellohel", "String concatenation should combine contents");

    const char* rawString = stringValue;
    TEST_ASSERT(0 == strcmp(rawString, "hello"), "String implicit const char* conversion should work");

    char* mutableRawString = stringValue;
    mutableRawString[1] = 'a';
    TEST_ASSERT(stringValue == "hallo", "String implicit char* conversion should allow mutation");

    return Result<void>::make();
}
