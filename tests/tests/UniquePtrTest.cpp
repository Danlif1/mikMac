//
//  UniquePtrTest.cpp
//  tests
//
#include "Framework/TestFramework.hpp"

#include "Pointers/UniquePtr.hpp"


using namespace dstd;
using namespace tests;

namespace {

struct TrackedObject {
    static int s_aliveCount;

    int value;

    explicit TrackedObject(int value)
        : value(value)
    {
        ++s_aliveCount;
    }

    ~TrackedObject() {
        --s_aliveCount;
    }
};

int TrackedObject::s_aliveCount = 0;

} // namespace

dstd::Result<void> tests::testUniquePtr() {
    static_assert(is_same<Result<UniquePtr<TrackedObject>>, decltype(UniquePtr<TrackedObject>::make(1))>::value,
                  "UniquePtr::make interface changed");

    TrackedObject::s_aliveCount = 0;

    CHECK_RESULT(uniquePointer, UniquePtr<TrackedObject>::make(11), "UniquePtr::make failed");
    TEST_ASSERT_EQ(1, TrackedObject::s_aliveCount, "UniquePtr should own one object");
    TEST_ASSERT(nullptr != uniquePointer.getValue(), "UniquePtr should expose stored pointer");
    TEST_ASSERT_EQ(11, uniquePointer.getValue()->value, "UniquePtr should construct stored object");

    UniquePtr<TrackedObject> movedPointer = move(uniquePointer);
    TEST_ASSERT(nullptr == uniquePointer.getValue(), "moved-from UniquePtr should release ownership");
    TEST_ASSERT(nullptr != movedPointer.getValue(), "moved-to UniquePtr should keep ownership");
    TEST_ASSERT_EQ(1, TrackedObject::s_aliveCount, "move should not duplicate owned object");

    TrackedObject* releasedPointer = movedPointer.release();
    TEST_ASSERT(nullptr == movedPointer.getValue(), "release should clear UniquePtr");
    TEST_ASSERT(nullptr != releasedPointer, "release should return raw pointer");
    delete releasedPointer;
    TEST_ASSERT_EQ(0, TrackedObject::s_aliveCount, "released object should be destroyed manually");

    CHECK_RESULT(factoryPointer, makeUnique<TrackedObject>(22), "makeUnique failed");
    TEST_ASSERT_EQ(1, TrackedObject::s_aliveCount, "makeUnique should create one object");
    TEST_ASSERT_EQ(22, factoryPointer->value, "makeUnique should forward constructor arguments");

    factoryPointer = move(uniquePointer);
    TEST_ASSERT_EQ(0, TrackedObject::s_aliveCount, "move assignment should destroy previous owned object");

    CHECK_RESULT(replacementPointer, UniquePtr<TrackedObject>::make(33), "UniquePtr::make failed for replacement");
    factoryPointer = move(replacementPointer);
    TEST_ASSERT_EQ(1, TrackedObject::s_aliveCount, "replacement UniquePtr should own one object");

    return Result<void>::make();
}
