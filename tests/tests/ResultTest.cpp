//
//  ResultTest.cpp
//  tests
//
#include "Framework/TestFramework.hpp"

#include "Result.hpp"


using namespace dstd;
using namespace tests;

namespace {

struct Payload {
    int value;
};

} // namespace

dstd::Result<void> tests::testResult() {
    static_assert(is_same<Result<int>, decltype(Result<int>::make(1))>::value, "Result::make interface changed");

    Result<int> emptyResult;
    TEST_ASSERT(emptyResult.hasError(), "default Result<T> should be an error");
    TEST_ASSERT_EQ(KERN_FAILURE, emptyResult.error(), "default Result<T> should use KERN_FAILURE");

    Result<int> successResult = Result<int>::make(42);
    TEST_ASSERT(successResult.hasValue(), "Result::make should succeed");
    TEST_ASSERT_EQ(42, successResult.value(), "Result should store moved value");

    Result<int> errorResult = Result<int>::makeError(KERN_INVALID_ARGUMENT);
    TEST_ASSERT(errorResult.hasError(), "Result::makeError should fail");
    TEST_ASSERT_EQ(KERN_INVALID_ARGUMENT, errorResult.error(), "Result should preserve error code");

    const Result<void> voidSuccess = Result<void>::make();
    TEST_ASSERT(voidSuccess.hasValue(), "Result<void>::make should succeed");
    TEST_ASSERT_EQ(KERN_SUCCESS, voidSuccess.value(), "Result<void> should report KERN_SUCCESS");

    const Result<void> voidError = Result<void>::makeError(KERN_NOT_FOUND);
    TEST_ASSERT(voidError.hasError(), "Result<void>::makeError should fail");
    TEST_ASSERT_EQ(KERN_NOT_FOUND, voidError.error(), "Result<void> should preserve error code");

    Result<Payload> convertedError(voidError);
    TEST_ASSERT(convertedError.hasError(), "Result<T> should convert void error");
    TEST_ASSERT_EQ(KERN_NOT_FOUND, convertedError.error(), "converted Result<T> should preserve error code");

    Result<int> movedSuccess = move(successResult);
    TEST_ASSERT(movedSuccess.hasValue(), "move-constructed Result should keep value");
    TEST_ASSERT_EQ(42, movedSuccess.value(), "move-constructed Result should keep payload");

    return Result<void>::make();
}
