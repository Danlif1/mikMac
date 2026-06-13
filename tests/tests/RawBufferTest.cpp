//
//  RawBufferTest.cpp
//  tests
//
#include "Framework/TestFramework.hpp"

#include "Pointers/RawBuffer.hpp"

#include <string.h>


using namespace dstd;
using namespace tests;

dstd::Result<void> tests::testRawBuffer() {
    static_assert(is_same<Result<RawBuffer>, decltype(RawBuffer::make())>::value, "RawBuffer::make interface changed");
    static_assert(is_same<Result<RawBuffer>, decltype(RawBuffer::make(16))>::value, "RawBuffer::make(size) interface changed");

    CHECK_RESULT(emptyBuffer, RawBuffer::make(), "RawBuffer::make failed");
    TEST_ASSERT_EQ(0, emptyBuffer.size(), "default RawBuffer should have zero size");

    CHECK_RESULT(buffer, RawBuffer::make(8), "RawBuffer::make(size) failed");
    TEST_ASSERT_EQ(8, buffer.size(), "RawBuffer should allocate requested size");

    memset(buffer.getCharBuffer(), 'a', buffer.size());
    CHECK_RESULT(copiedBuffer, RawBuffer::copy(buffer), "RawBuffer::copy failed");
    TEST_ASSERT_EQ(0, memcmp(buffer.get(), copiedBuffer.get(), buffer.size()), "RawBuffer::copy should duplicate bytes");

    CHECK_RESULT_NO_VALUE(buffer.append(RawBufferView("123", 3)), "RawBuffer append view failed");
    CHECK_RESULT_NO_VALUE(buffer.append(RawBufferView("456", 3)), "RawBuffer append view failed second time");
    TEST_ASSERT(buffer.size() >= 6, "RawBuffer append should preserve appended bytes");

    CHECK_RESULT_NO_VALUE(buffer.resize(4), "RawBuffer shrink failed");
    TEST_ASSERT_EQ(4, buffer.size(), "RawBuffer resize should shrink allocation");
    TEST_ASSERT_EQ('1', buffer.getCharBuffer()[0], "RawBuffer shrink should preserve prefix bytes");

    CHECK_RESULT_NO_VALUE(buffer.resize(16), "RawBuffer grow failed");
    TEST_ASSERT_EQ(16, buffer.size(), "RawBuffer resize should grow allocation");
    TEST_ASSERT_EQ('1', buffer.getCharBuffer()[0], "RawBuffer grow should preserve existing bytes");

    copiedBuffer.clear();
    TEST_ASSERT_EQ(0, copiedBuffer.getCharBuffer()[0], "RawBuffer clear should zero first byte");

    CHECK_RESULT_NO_VALUE(buffer.append(move(copiedBuffer)), "RawBuffer append move failed");

    return Result<void>::make();
}
