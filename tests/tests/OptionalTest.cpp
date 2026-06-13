//
//  OptionalTest.cpp
//  tests
//
#include "Framework/TestFramework.hpp"

#include "Optional.hpp"


using namespace dstd;
using namespace tests;

dstd::Result<void> tests::testOptional() {
    Optional<int> emptyOptional;
    TEST_ASSERT(!emptyOptional.hasValue(), "default Optional should be empty");

    Optional<int> valueOptional(7);
    TEST_ASSERT(valueOptional.hasValue(), "Optional constructed with value should be populated");
    TEST_ASSERT_EQ(7, valueOptional.value(), "Optional should store value");

    Optional<int> copiedOptional(valueOptional);
    TEST_ASSERT(copiedOptional.hasValue(), "copy-constructed Optional should be populated");
    TEST_ASSERT_EQ(7, copiedOptional.value(), "copy-constructed Optional should copy value");

    Optional<int> movedOptional(move(valueOptional));
    TEST_ASSERT(movedOptional.hasValue(), "move-constructed Optional should be populated");
    TEST_ASSERT_EQ(7, movedOptional.value(), "move-constructed Optional should steal value");
    TEST_ASSERT(!valueOptional.hasValue(), "moved-from Optional should be empty");
    // return the optional back for more testing.
    valueOptional = move(movedOptional);
    
    Optional<int> assignedOptional;
    assignedOptional = 9;
    TEST_ASSERT(assignedOptional.hasValue(), "assignment from value should populate Optional");
    TEST_ASSERT_EQ(9, assignedOptional.value(), "assignment from value should store value");

    Optional<int> reassignedOptional(1);
    reassignedOptional = assignedOptional;
    TEST_ASSERT(reassignedOptional.hasValue(), "assignment from Optional should populate Optional");
    TEST_ASSERT_EQ(9, reassignedOptional.value(), "assignment from Optional should copy value");

    Optional<int> moveAssignedOptional(1);
    moveAssignedOptional = move(movedOptional);
    TEST_ASSERT(moveAssignedOptional.hasValue(), "move assignment should populate Optional");
    TEST_ASSERT_EQ(7, moveAssignedOptional.value(), "move assignment should steal value");
    TEST_ASSERT(!movedOptional.hasValue(), "move-assigned-from Optional should be empty");

    reassignedOptional.reset();
    TEST_ASSERT(!reassignedOptional.hasValue(), "reset Optional should clear value");

    TEST_ASSERT(valueOptional == Optional<int>(7), "Optional should compare equal to matching value");
    TEST_ASSERT(emptyOptional != Optional<int>(7), "empty Optional should not compare equal to value");

    return Result<void>::make();
}
