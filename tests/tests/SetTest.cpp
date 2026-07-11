//
//  SetTest.cpp
//  tests
//
#include "Framework/TestFramework.hpp"

#include "DataStructures/Arrays/Set.hpp"


using namespace dstd;
using namespace tests;

dstd::Result<void> tests::testSet() {
    static_assert(is_same<Result<Set<int>>, decltype(Set<int>::make())>::value, "Set::make interface changed");

    CHECK_RESULT(setValue, Set<int>::make(), "Set::make failed");
    TEST_ASSERT(setValue.empty(), "default Set should be empty");
    TEST_ASSERT_EQ(0, setValue.size(), "default Set size should be zero");
    TEST_ASSERT(!setValue.has(1), "empty Set should not contain values");

    CHECK_RESULT_NO_VALUE(setValue.push(1), "Set push failed");
    CHECK_RESULT_NO_VALUE(setValue.push(2), "Set second push failed");
    CHECK_RESULT(insertedAgain, setValue.push(2), "Set duplicate push failed");
    TEST_ASSERT(!insertedAgain, "Set duplicate push should report no insertion");
    TEST_ASSERT_EQ(2, setValue.size(), "Set push should ignore duplicates");
    TEST_ASSERT(setValue.has(1), "Set has should find inserted value");
    TEST_ASSERT(setValue.has(2), "Set has should find inserted value");
    TEST_ASSERT(!setValue.has(3), "Set has should reject missing value");

    int foundOne = 0;
    int foundTwo = 0;
    for (const int element : setValue) {
        if (1 == element) {
            ++foundOne;
        } else if (2 == element) {
            ++foundTwo;
        } else {
            TEST_FAIL("Set range-for should iterate stored values only");
        }
    }
    TEST_ASSERT_EQ(1, foundOne, "Set range-for should visit every stored value");
    TEST_ASSERT_EQ(1, foundTwo, "Set range-for should visit every stored value");

    Set<int> movedSet = move(setValue);
    CHECK_RESULT_NO_VALUE(movedSet.push(99), "Set move should preserve usable state");
    TEST_ASSERT(movedSet.has(99), "Set move should preserve usable storage");
    TEST_ASSERT_EQ(3, movedSet.size(), "Set move should preserve existing elements");

    CHECK_RESULT_NO_VALUE(movedSet.clear(), "Set clear failed");
    TEST_ASSERT(movedSet.empty(), "Set clear should empty container");

    return Result<void>::make();
}
