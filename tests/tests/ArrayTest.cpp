//
//  ArrayTest.cpp
//  tests
//
#include "Framework/TestFramework.hpp"

#include "DataStructures/Arrays/Array.hpp"
#include "TypeTraites/DefaultMakeable.hpp"


using namespace dstd;
using namespace tests;

namespace {

class MakeableValue {
public:
    static Result<MakeableValue> make() {
        return Result<MakeableValue>::make(MakeableValue());
    }

    int value() const {
        return m_value;
    }

private:
    MakeableValue()
        : m_value(42)
    {}

    int m_value;
};

static_assert(DefaultMakeable<MakeableValue>::value, "MakeableValue should be DefaultMakeable");
static_assert(!DefaultMakeable<int>::value, "int should not be DefaultMakeable");

} // namespace

dstd::Result<void> tests::testArray() {
    static_assert(is_same<Result<Array<int, 3>>, decltype(Array<int, 3>::make())>::value, "Array::make interface changed");
    static_assert(Array<int, 3>::size() == 3, "Array::size interface changed");

    CHECK_RESULT(arrayValue, (Array<int, 3>::make()), "Array::make failed");
    TEST_ASSERT_EQ(3, arrayValue.size(), "Array size should match compile-time size");
    TEST_ASSERT(!arrayValue.empty(), "non-zero Array should not be empty");

    CHECK_RESULT_NO_VALUE(arrayValue.fill(7), "Array fill failed");
    TEST_ASSERT_EQ(7, arrayValue[0], "Array fill should populate first element");
    TEST_ASSERT_EQ(7, arrayValue.back(), "Array fill should populate last element");

    size_t iteratedCount = 0;
    for (const int element : arrayValue) {
        TEST_ASSERT_EQ(7, element, "Array range-for should iterate filled values");
        ++iteratedCount;
    }
    TEST_ASSERT_EQ(3, iteratedCount, "Array range-for should visit every element");

    CHECK_RESULT(argumentArray, (Array<int, 2>::make(9)), "Array::make(args) failed");
    TEST_ASSERT_EQ(9, argumentArray.front(), "Array::make(args) should construct every element");

    CHECK_RESULT(makeableArray, (Array<MakeableValue, 2>::make()), "Array::make for DefaultMakeable failed");
    TEST_ASSERT_EQ(42, makeableArray[0].value(), "DefaultMakeable Array should construct using make()");
    TEST_ASSERT_EQ(42, makeableArray[1].value(), "DefaultMakeable Array should construct every element");

    Array<int, 3> movedArray = move(arrayValue);
    TEST_ASSERT_EQ(7, movedArray[1], "moved Array should preserve elements");

    return Result<void>::make();
}
