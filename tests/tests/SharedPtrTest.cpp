//
//  SharedPtrTest.cpp
//  tests
//
#include "Framework/TestFramework.hpp"

#include "Pointers/SharedPtr.hpp"


using namespace dstd;
using namespace tests;

namespace {

struct SharedPayload {
    static int s_aliveCount;

    int value;

    explicit SharedPayload(int value)
        : value(value)
    {
        ++s_aliveCount;
    }

    ~SharedPayload() {
        --s_aliveCount;
    }
};

int SharedPayload::s_aliveCount = 0;

} // namespace

dstd::Result<void> tests::testSharedPtr() {
    static_assert(is_same<Result<SharedPtr<SharedPayload>>, decltype(makeShared<SharedPayload>(1))>::value,
                  "makeShared interface changed");

    SharedPayload::s_aliveCount = 0;

    {
        CHECK_RESULT(firstShared, makeShared<SharedPayload>(5), "makeShared failed");
        TEST_ASSERT_EQ(1, SharedPayload::s_aliveCount, "SharedPtr should construct one object");
        TEST_ASSERT_EQ(1, firstShared.useCount(), "single SharedPtr should have use count 1");
        TEST_ASSERT_EQ(5, firstShared.getValue()->value, "SharedPtr should expose constructed value");

        SharedPtr<SharedPayload> secondShared(firstShared);
        TEST_ASSERT_EQ(1, SharedPayload::s_aliveCount, "copying SharedPtr should not allocate a new object");
        TEST_ASSERT_EQ(2, firstShared.useCount(), "copied SharedPtr should increment use count");
        TEST_ASSERT_EQ(5, secondShared.getValue()->value, "copied SharedPtr should alias same object");

        SharedPtr<SharedPayload> thirdShared(firstShared);
        SharedPtr<SharedPayload> fourthShared(firstShared);
        fourthShared = secondShared;
        TEST_ASSERT_EQ(4, firstShared.useCount(), "assignment should increment use count");
        TEST_ASSERT_EQ(1, SharedPayload::s_aliveCount, "assignment should not allocate a new object");

        CHECK_RESULT(replacementShared, makeShared<SharedPayload>(9), "makeShared failed for replacement");
        firstShared.replaceValue(move(replacementShared));
        TEST_ASSERT_EQ(2, SharedPayload::s_aliveCount, "replaceValue should keep old and new objects alive");
        TEST_ASSERT_EQ(1, firstShared.useCount(), "replaceValue should reset use count on destination");
        TEST_ASSERT_EQ(9, firstShared.getValue()->value, "replaceValue should install new value");
    }

    TEST_ASSERT_EQ(0, SharedPayload::s_aliveCount, "destroying all SharedPtr instances should destroy payload");

    return Result<void>::make();
}
