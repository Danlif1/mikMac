//
//  SingletonTest.cpp
//  tests
//
//  Created by Daniel Lifshitz on 13/06/2026.
//
#include "Framework/TestFramework.hpp"

#include "DataStructures/Arrays/Vector.hpp"
#include "DataStructures/Singleton.hpp"

#include <sys/errno.h>


using namespace dstd;
using namespace tests;

namespace {

struct SingletonPayload {
    static int s_aliveCount;

    int value;

    explicit SingletonPayload(int value)
        : value(value)
    {
        ++s_aliveCount;
    }

    ~SingletonPayload() {
        --s_aliveCount;
    }
};

int SingletonPayload::s_aliveCount = 0;

Result<void> testSingletonUnavailableBeforeInit() {
    const auto missingSingleton = Singleton<int>::get();
    TEST_ASSERT(missingSingleton.hasError(), "Singleton should not exist before initialization");
    TEST_ASSERT_EQ(ENOENT, missingSingleton.error(), "Singleton get should return ENOENT before initialization");

    return Result<void>::make();
}

Result<void> testSingletonScopeSurvivesInitScope() {
    Optional<SingletonGuard> ownedGuard;

    {
        CHECK_RESULT(localGuard, Singleton<int>::make(42), "Singleton make failed in init scope");
        ownedGuard = Optional<SingletonGuard>(move(localGuard));

        CHECK_RESULT(singletonInScope, Singleton<int>::get(), "Singleton should be available in init scope");
        TEST_ASSERT_EQ(42, *singletonInScope, "Singleton should keep value in init scope");
    }

    CHECK_RESULT(singletonOutOfScope, Singleton<int>::get(), "Singleton should survive init scope");
    TEST_ASSERT_EQ(42, *singletonOutOfScope, "Singleton value should survive init scope");

    ownedGuard.reset();

    const auto missingSingleton = Singleton<int>::get();
    TEST_ASSERT(missingSingleton.hasError(), "Singleton should be destroyed with its guard");
    TEST_ASSERT_EQ(ENOENT, missingSingleton.error(), "Singleton get should return ENOENT after guard destruction");

    return Result<void>::make();
}

Result<void> testSingletonGuardVector() {
    CHECK_RESULT(guards, Vector<SingletonGuard>::make(), "Failed to allocate singleton guard vector");

    {
        CHECK_RESULT(intGuard, Singleton<int>::make(7), "Failed to create int singleton");
        CHECK_RESULT_NO_VALUE(guards.push_back(move(intGuard)), "Failed to store int singleton guard");

        CHECK_RESULT(payloadGuard, Singleton<SingletonPayload>::make(11), "Failed to create payload singleton");
        CHECK_RESULT_NO_VALUE(guards.push_back(move(payloadGuard)), "Failed to store payload singleton guard");
    }

    TEST_ASSERT_EQ(2, guards.size(), "Singleton guard vector should hold two guards");
    TEST_ASSERT_EQ(1, SingletonPayload::s_aliveCount, "Payload singleton should be alive once");

    CHECK_RESULT(intSingleton, Singleton<int>::get(), "Int singleton should be available through guard vector");
    CHECK_RESULT(payloadSingleton, Singleton<SingletonPayload>::get(), "Payload singleton should be available through guard vector");
    TEST_ASSERT_EQ(7, *intSingleton, "Int singleton should keep stored value");
    TEST_ASSERT_EQ(11, payloadSingleton->value, "Payload singleton should keep stored value");

    CHECK_RESULT_NO_VALUE(guards.clear(), "Failed to clear singleton guard vector");

    TEST_ASSERT_EQ(0, SingletonPayload::s_aliveCount, "Clearing guard vector should destroy payload singleton");

    const auto missingIntSingleton = Singleton<int>::get();
    TEST_ASSERT(missingIntSingleton.hasError(), "Int singleton should be destroyed when guard vector is cleared");
    TEST_ASSERT_EQ(ENOENT, missingIntSingleton.error(), "Int singleton get should return ENOENT after guard destruction");

    const auto missingPayloadSingleton = Singleton<SingletonPayload>::get();
    TEST_ASSERT(missingPayloadSingleton.hasError(), "Payload singleton should be destroyed when guard vector is cleared");

    return Result<void>::make();
}

Result<void> testSingletonMakeIsExclusive() {
    CHECK_RESULT(firstGuard, Singleton<int>::make(1), "First singleton make failed");

    const auto duplicateMake = Singleton<int>::make(2);
    TEST_ASSERT(duplicateMake.hasError(), "Second singleton make should fail");
    TEST_ASSERT_EQ(EEXIST, duplicateMake.error(), "Second singleton make should return EEXIST");

    return Result<void>::make();
}

} // namespace

dstd::Result<void> tests::testSingleton() {
    SingletonPayload::s_aliveCount = 0;

    CHECK_RESULT_NO_VALUE(testSingletonUnavailableBeforeInit(), "Singleton availability before init test failed");
    CHECK_RESULT_NO_VALUE(testSingletonScopeSurvivesInitScope(), "Singleton init scope test failed");
    CHECK_RESULT_NO_VALUE(testSingletonGuardVector(), "Singleton guard vector test failed");
    CHECK_RESULT_NO_VALUE(testSingletonMakeIsExclusive(), "Singleton exclusive make test failed");

    TEST_ASSERT_EQ(0, SingletonPayload::s_aliveCount, "All singleton payload instances should be destroyed");

    return Result<void>::make();
}
