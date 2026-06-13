//
//  VectorTest.cpp
//  tests
//
#include "Framework/TestFramework.hpp"

#include "DataStructures/Arrays/Vector.hpp"


using namespace dstd;
using namespace tests;

dstd::Result<void> tests::testVector() {
    static_assert(is_same<Result<Vector<int>>, decltype(Vector<int>::make())>::value, "Vector::make interface changed");
    static_assert(is_same<Result<Vector<int>>, decltype(Vector<int>::make(4))>::value, "Vector::make(capacity) interface changed");

    CHECK_RESULT(vectorValue, Vector<int>::make(), "Vector::make failed");
    TEST_ASSERT(vectorValue.empty(), "default Vector should be empty");
    TEST_ASSERT_EQ(0, vectorValue.size(), "default Vector size should be zero");

    CHECK_RESULT_NO_VALUE(vectorValue.push_back(1), "Vector push_back failed");
    CHECK_RESULT_NO_VALUE(vectorValue.push_back(2), "Vector second push_back failed");
    CHECK_RESULT_NO_VALUE(vectorValue.push_back(3), "Vector third push_back failed");
    TEST_ASSERT_EQ(3, vectorValue.size(), "Vector push_back should increase size");
    TEST_ASSERT_EQ(1, vectorValue[0], "Vector operator[] should expose first element");
    TEST_ASSERT_EQ(3, vectorValue.back(), "Vector back should expose last element");

    size_t iteratedCount = 0;
    for (const int element : vectorValue) {
        TEST_ASSERT_EQ(static_cast<int>(iteratedCount + 1), element, "Vector range-for should iterate in order");
        ++iteratedCount;
    }
    TEST_ASSERT_EQ(3, iteratedCount, "Vector range-for should visit every element");

    CHECK_RESULT_NO_VALUE(vectorValue.reserve(16), "Vector reserve failed");
    TEST_ASSERT(vectorValue.capacity() >= 16, "Vector reserve should increase capacity");
    TEST_ASSERT_EQ(3, vectorValue.size(), "Vector reserve should not change size");

    CHECK_RESULT_NO_VALUE(vectorValue.resize(5), "Vector resize grow failed");
    TEST_ASSERT_EQ(5, vectorValue.size(), "Vector resize should grow size");
    TEST_ASSERT_EQ(3, vectorValue[2], "Vector resize grow should preserve existing elements");

    CHECK_RESULT_NO_VALUE(vectorValue.resize(2), "Vector resize shrink failed");
    TEST_ASSERT_EQ(2, vectorValue.size(), "Vector resize should shrink size");
    TEST_ASSERT_EQ(2, vectorValue[1], "Vector resize shrink should preserve remaining elements");

    CHECK_RESULT_NO_VALUE(vectorValue.pop_back(), "Vector pop_back failed");
    TEST_ASSERT_EQ(1, vectorValue.size(), "Vector pop_back should reduce size");

    CHECK_RESULT(reservedVector, Vector<int>::make(4), "Vector::make(capacity) failed");
    TEST_ASSERT(reservedVector.capacity() >= 4, "Vector::make(capacity) should reserve storage");

    Vector<int> movedVector = move(reservedVector);
    CHECK_RESULT_NO_VALUE(movedVector.push_back(99), "Vector move should preserve usable state");
    TEST_ASSERT_EQ(99, movedVector[0], "Vector move should preserve usable storage");

    CHECK_RESULT_NO_VALUE(movedVector.clear(), "Vector clear failed");
    TEST_ASSERT(movedVector.empty(), "Vector clear should empty container");

    return Result<void>::make();
}
