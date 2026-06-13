//
//  TupleTest.cpp
//  tests
//
#include "Framework/TestFramework.hpp"

#include "DataStructures/Tuple.hpp"

#include <string.h>


using namespace dstd;
using namespace tests;

dstd::Result<void> tests::testTuple() {
    static_assert(tuple_size_v<Tuple<int, const char*, bool>> == 3, "tuple_size interface changed");
    static_assert(is_same<int, tuple_element_t<0, Tuple<int, const char*, bool>>>::value, "tuple_element interface changed");

    Tuple<int, const char*, bool> tupleValue(1, "two", true);
    TEST_ASSERT_EQ(1, get<0>(tupleValue), "Tuple get<0> should return first element");
    TEST_ASSERT_EQ(0, strcmp(get<1>(tupleValue), "two"), "Tuple get<1> should return second element");
    TEST_ASSERT(get<2>(tupleValue), "Tuple get<2> should return third element");

    get<0>(tupleValue) = 3;
    get<2>(tupleValue) = false;
    TEST_ASSERT_EQ(3, get<0>(tupleValue), "Tuple element assignment should mutate stored value");
    TEST_ASSERT(!get<2>(static_cast<const Tuple<int, const char*, bool>&>(tupleValue)), "const Tuple access should observe mutation");

    Tuple<int, const char*, bool> movedTuple = move(tupleValue);
    get<0>(movedTuple) = 7;
    TEST_ASSERT_EQ(7, get<0>(movedTuple), "moved Tuple should remain usable");

    Tuple<> emptyTuple;
    static_assert(tuple_size_v<Tuple<>> == 0, "empty Tuple should have zero size");

    return Result<void>::make();
}
