//
//  LockTest.cpp
//  tests
//
#include "Framework/TestFramework.hpp"

#include "DataStructures/Map.hpp"
#include "Synchronization/Lock.hpp"
#include "Synchronization/Locker.hpp"


using namespace dstd;
using namespace tests;

namespace {

struct Counter {
    int m_exclusiveCount;
    int m_sharedReads;
};

Result<void> testLockExclusiveAndShared() {
    CHECK_RESULT(lockValue, Lock::make("tests.lock"), "Lock::make failed");

    lockValue.lockExclusive();
    lockValue.unlockExclusive();

    lockValue.lockShared();
    lockValue.unlockShared();

    return {};
}

Result<void> testLockerExclusiveGuard() {
    CHECK_RESULT(lockerValue, Locker<Counter>::make(Counter {0, 0}, "tests.locker"), "Locker::make failed");

    {
        auto exclusiveGuard = lockerValue.lockExclusive();
        exclusiveGuard->m_exclusiveCount = 7;
        exclusiveGuard.get().m_sharedReads = 3;
    }

    auto sharedGuard = lockerValue.lockShared();
    TEST_ASSERT_EQ(7, sharedGuard->m_exclusiveCount, "exclusive guard should update protected object");
    TEST_ASSERT_EQ(3, sharedGuard->m_sharedReads, "exclusive guard should update protected object");

    return {};
}

Result<void> testLockerProtectsMap() {
    using TestMap = Map<uint64_t, int>;
    CHECK_RESULT(mapValue, (TestMap::make()), "Map::make failed");
    CHECK_RESULT(lockerValue, Locker<TestMap>::make(move(mapValue), "tests.map.locker"), "Locker::make failed");

    {
        auto exclusiveGuard = lockerValue.lockExclusive();
        CHECK_RESULT_NO_VALUE(exclusiveGuard->insert(42ULL, 100), "insert under exclusive lock failed");
    }

    {
        auto sharedGuard = lockerValue.lockShared();
        const int* value = sharedGuard->find(42ULL);
        TEST_ASSERT(nullptr != value, "shared guard should read protected map");
        TEST_ASSERT_EQ(100, *value, "shared guard should observe inserted value");
    }

    return {};
}

} // namespace

dstd::Result<void> tests::testLock() {
    CHECK_RESULT_NO_VALUE(testLockExclusiveAndShared(), "lock exclusive/shared test failed");
    CHECK_RESULT_NO_VALUE(testLockerExclusiveGuard(), "locker exclusive guard test failed");
    CHECK_RESULT_NO_VALUE(testLockerProtectsMap(), "locker map protection test failed");
    return {};
}
