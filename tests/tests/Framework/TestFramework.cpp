//
//  TestFramework.cpp
//  tests
//
#include "TestFramework.hpp"


namespace tests {

namespace {

dstd::Result<void> runTest(const char* name, dstd::Result<void> (*testFunction)()) {
    LOG(dstd::LogLevel::LOG_INFO, "tests: running %s", name);
    const dstd::Result<void> result = testFunction();
    if (result.hasError()) {
        LOG(dstd::LogLevel::LOG_ERROR, "tests: %s failed with error %d", name, result.error());
        return result;
    }

    LOG(dstd::LogLevel::LOG_INFO, "tests: %s passed", name);
    return dstd::Result<void>::make();
}

} // namespace

#define RUN_TEST(name, testFunction) \
    if (runTest(name, testFunction).hasError()) { \
        anyFailed = true; \
    }

dstd::Result<void> runAllTests() {
    bool anyFailed = false;

    RUN_TEST("Result", testResult);
    RUN_TEST("Optional", testOptional);
    RUN_TEST("UniquePtr", testUniquePtr);
    RUN_TEST("SharedPtr", testSharedPtr);
    RUN_TEST("RawBuffer", testRawBuffer);
    RUN_TEST("String", testString);
    RUN_TEST("Vector", testVector);
    RUN_TEST("Array", testArray);
    RUN_TEST("Tuple", testTuple);
    RUN_TEST("Map", testMap);
    RUN_TEST("Singleton", testSingleton);
    RUN_TEST("File", testFile);

    if (anyFailed) {
        LOG(dstd::LogLevel::LOG_ERROR, "tests: one or more test suites failed");
        return dstd::Result<void>::makeError(KERN_FAILURE);
    }

    LOG(dstd::LogLevel::LOG_INFO, "tests: all tests passed");
    return dstd::Result<void>::make();
}

} // namespace tests
