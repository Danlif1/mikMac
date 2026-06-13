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

dstd::Result<void> runAllTests() {
    CHECK_RESULT_NO_VALUE(runTest("Result", testResult), "Result tests failed");
    CHECK_RESULT_NO_VALUE(runTest("Optional", testOptional), "Optional tests failed");
    CHECK_RESULT_NO_VALUE(runTest("UniquePtr", testUniquePtr), "UniquePtr tests failed");
    CHECK_RESULT_NO_VALUE(runTest("SharedPtr", testSharedPtr), "SharedPtr tests failed");
    CHECK_RESULT_NO_VALUE(runTest("RawBuffer", testRawBuffer), "RawBuffer tests failed");
    CHECK_RESULT_NO_VALUE(runTest("String", testString), "String tests failed");
    CHECK_RESULT_NO_VALUE(runTest("Vector", testVector), "Vector tests failed");
    CHECK_RESULT_NO_VALUE(runTest("Array", testArray), "Array tests failed");
    CHECK_RESULT_NO_VALUE(runTest("Tuple", testTuple), "Tuple tests failed");
    CHECK_RESULT_NO_VALUE(runTest("Map", testMap), "Map tests failed");

    LOG(dstd::LogLevel::LOG_INFO, "tests: all tests passed");
    return dstd::Result<void>::make();
}

} // namespace tests
