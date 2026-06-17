//
//  TestFramework.hpp
//  tests
//
//  Created by Daniel Lifshitz on 13/06/2026.
//
#pragma once

#include <mach/mach_types.h>

#include "Checkers.hpp"
#include "Logger.hpp"
#include "Result.hpp"
#include "TypeTraites/TypeTraites.hpp"
#include "Files/FileLogger.hpp"



namespace tests {

dstd::Result<void> runAllTests();

#define TEST_FAIL(message) \
    { \
        LOG(dstd::LogLevel::LOG_ERROR, "tests: FAIL: " message); \
        return dstd::Result<void>::makeError(KERN_FAILURE); \
    }

#define TEST_ASSERT(condition, message) \
    { \
        if (!(condition)) { \
            TEST_FAIL(message); \
        } \
    }

#define TEST_ASSERT_EQ(expected, actual, message) \
    { \
        const auto expectedValue = (expected); \
        const auto actualValue = (actual); \
        if (expectedValue != actualValue) { \
            LOG(dstd::LogLevel::LOG_ERROR, \
                "tests: FAIL: " message " (expected %lld got %lld)", \
                static_cast<long long>(expectedValue), \
                static_cast<long long>(actualValue)); \
            return dstd::Result<void>::makeError(KERN_FAILURE); \
        } \
    }

dstd::Result<void> testResult();
dstd::Result<void> testOptional();
dstd::Result<void> testUniquePtr();
dstd::Result<void> testSharedPtr();
dstd::Result<void> testRawBuffer();
dstd::Result<void> testString();
dstd::Result<void> testVector();
dstd::Result<void> testArray();
dstd::Result<void> testTuple();
dstd::Result<void> testMap();
dstd::Result<void> testSingleton();
dstd::Result<void> testFile();

} // namespace tests
