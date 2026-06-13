//
//  MapTest.cpp
//  tests
//
#include "Framework/TestFramework.hpp"

#include "DataStructures/Map.hpp"


using namespace dstd;
using namespace tests;

dstd::Result<void> tests::testMap() {
    static_assert(is_same<Result<Map<uint64_t, int>>, decltype(Map<uint64_t, int>::make())>::value, "Map::make interface changed");
    static_assert(is_same<Result<Map<uint64_t, int>>, decltype(Map<uint64_t, int>::make(16))>::value,
                  "Map::make(capacity) interface changed");

    CHECK_RESULT(mapValue, (Map<uint64_t, int>::make()), "Map::make failed");
    TEST_ASSERT(mapValue.empty(), "new Map should be empty");
    TEST_ASSERT(mapValue.capacity() >= 8, "Map should normalize default capacity");

    CHECK_RESULT_NO_VALUE(mapValue.insert(10ULL, 100), "Map insert failed");
    CHECK_RESULT_NO_VALUE(mapValue.insert(20ULL, 200), "Map second insert failed");
    TEST_ASSERT_EQ(2, mapValue.size(), "Map insert should increase size");
    TEST_ASSERT(mapValue.contains(10ULL), "Map contains should find inserted key");
    TEST_ASSERT(!mapValue.contains(99ULL), "Map contains should reject missing key");

    int* firstValue = mapValue.find(10ULL);
    TEST_ASSERT(nullptr != firstValue, "Map find should return value pointer");
    TEST_ASSERT_EQ(100, *firstValue, "Map find should return inserted value");

    CHECK_RESULT_NO_VALUE(mapValue.insert(10ULL, 111), "Map update insert failed");
    TEST_ASSERT_EQ(2, mapValue.size(), "Map update should not increase size");
    TEST_ASSERT_EQ(111, *mapValue.find(10ULL), "Map insert should update existing value");

    const Map<uint64_t, int>& constMap = mapValue;
    const int* constValue = constMap.find(20ULL);
    TEST_ASSERT(nullptr != constValue, "const Map find should return value pointer");
    TEST_ASSERT_EQ(200, *constValue, "const Map find should return inserted value");

    size_t iteratedCount = 0;
    for (const auto& entry : mapValue) {
        TEST_ASSERT(entry.isOccupied(), "Map iterator should only expose occupied entries");
        ++iteratedCount;
    }
    TEST_ASSERT_EQ(2, iteratedCount, "Map iterator should visit every occupied entry");

    CHECK_RESULT_NO_VALUE(mapValue.remove(10ULL), "Map remove failed");
    TEST_ASSERT_EQ(1, mapValue.size(), "Map remove should decrease size");
    TEST_ASSERT(nullptr == mapValue.find(10ULL), "Map remove should delete key");

    const Result<void> removeMissingResult = mapValue.remove(10ULL);
    TEST_ASSERT(removeMissingResult.hasError(), "Map remove missing key should return error");
    TEST_ASSERT_EQ(KERN_NOT_FOUND, removeMissingResult.error(), "Map remove missing key should use KERN_NOT_FOUND");

    for (uint64_t key = 0; key < 32; ++key) {
        CHECK_RESULT_NO_VALUE(mapValue.insert(key, static_cast<int>(key * 2)), "Map bulk insert failed");
    }
    TEST_ASSERT_EQ(32, mapValue.size(), "Map should survive rehash during bulk insert");

    for (uint64_t key = 0; key < 32; ++key) {
        int* value = mapValue.find(key);
        TEST_ASSERT(nullptr != value, "Map should find rehashed keys");
        TEST_ASSERT_EQ(static_cast<int>(key * 2), *value, "Map should preserve values across rehash");
    }

    CHECK_RESULT_NO_VALUE(mapValue.clear(), "Map clear failed");
    TEST_ASSERT(mapValue.empty(), "Map clear should empty container");
    TEST_ASSERT(nullptr == mapValue.find(0ULL), "Map clear should remove all keys");

    return Result<void>::make();
}
