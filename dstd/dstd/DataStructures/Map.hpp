//
//  Map.hpp
//  dstd
//
//  Created by Daniel Lifshitz on 13/06/2026.
//
#pragma once

#include <stdint.h>

#include "Checkers.hpp"
#include "Result.hpp"
#include "TypeTraites/TypeTraites.hpp"
#include "Pointers/RawBuffer.hpp"


namespace dstd {

enum class MapSlotState : uint8_t {
    Empty = 0,
    Occupied = 1,
    Tombstone = 2,
};

template<typename T>
struct MapHash {
    size_t operator()(const T& value) const {
        const size_t hashValue = static_cast<size_t>(value);
        return hashValue ^ (hashValue >> 16);
    }
};

template<typename T>
struct MapHash<T*> {
    size_t operator()(T* value) const {
        return MapHash<uintptr_t>()(reinterpret_cast<uintptr_t>(value));
    }
};

template<typename Key, typename Value>
class MapEntry {
public:
    MapEntry()
        : m_state(MapSlotState::Empty)
    {}

    ~MapEntry() {
        reset();
    }

    MapSlotState state() const {
        return m_state;
    }

    bool isOccupied() const {
        return MapSlotState::Occupied == m_state;
    }

    bool isEmpty() const {
        return MapSlotState::Empty == m_state;
    }

    bool isTombstone() const {
        return MapSlotState::Tombstone == m_state;
    }

    Key& key() {
        BREAK_IF_DEBUGGER_PRESENT(isOccupied());
        return *keyPtr();
    }

    const Key& key() const {
        BREAK_IF_DEBUGGER_PRESENT(isOccupied());
        return *keyPtr();
    }

    Value& value() {
        BREAK_IF_DEBUGGER_PRESENT(isOccupied());
        return *valuePtr();
    }

    const Value& value() const {
        BREAK_IF_DEBUGGER_PRESENT(isOccupied());
        return *valuePtr();
    }

    template<typename K, typename V>
    void construct(K&& key, V&& value) {
        reset();
        new (keyPtr()) Key(forward<K>(key));
        new (valuePtr()) Value(forward<V>(value));
        m_state = MapSlotState::Occupied;
    }

    void reset() {
        if (isOccupied()) {
            keyPtr()->~Key();
            valuePtr()->~Value();
        }

        m_state = MapSlotState::Empty;
    }

    void markTombstone() {
        if (isOccupied()) {
            keyPtr()->~Key();
            valuePtr()->~Value();
        }

        m_state = MapSlotState::Tombstone;
    }

    void abandon() {
        m_state = MapSlotState::Empty;
    }

private:
    Key* keyPtr() {
        return reinterpret_cast<Key*>(&m_keyStorage);
    }

    const Key* keyPtr() const {
        return reinterpret_cast<const Key*>(&m_keyStorage);
    }

    Value* valuePtr() {
        return reinterpret_cast<Value*>(&m_valueStorage);
    }

    const Value* valuePtr() const {
        return reinterpret_cast<const Value*>(&m_valueStorage);
    }

    MapSlotState m_state;
    alignas(Key) char m_keyStorage[sizeof(Key)];
    alignas(Value) char m_valueStorage[sizeof(Value)];
};

template<typename Key, typename Value>
class Map {
public:
    using key_type = Key;
    using mapped_type = Value;
    using value_type = MapEntry<Key, Value>;
    using size_type = size_t;

    struct Iterator {
        Map* m_map;
        size_t m_index;

        Iterator(Map* map, size_t index) noexcept
            : m_map(map)
            , m_index(index)
        {
            advanceToOccupied();
        }

        MapEntry<Key, Value>& operator*() const {
            return m_map->entries()[m_index];
        }

        MapEntry<Key, Value>* operator->() const {
            return &m_map->entries()[m_index];
        }

        Iterator& operator++() {
            ++m_index;
            advanceToOccupied();
            return *this;
        }

        bool operator==(const Iterator& other) const noexcept {
            return m_map == other.m_map && m_index == other.m_index;
        }

        bool operator!=(const Iterator& other) const noexcept {
            return !(*this == other);
        }

    private:
        void advanceToOccupied() {
            while (nullptr != m_map && m_index < m_map->m_capacity && !m_map->entries()[m_index].isOccupied()) {
                ++m_index;
            }
        }
    };

    struct ConstIterator {
        const Map* m_map;
        size_t m_index;

        ConstIterator(const Map* map, size_t index) noexcept
            : m_map(map)
            , m_index(index)
        {
            advanceToOccupied();
        }

        const MapEntry<Key, Value>& operator*() const {
            return m_map->entries()[m_index];
        }

        const MapEntry<Key, Value>* operator->() const {
            return &m_map->entries()[m_index];
        }

        ConstIterator& operator++() {
            ++m_index;
            advanceToOccupied();
            return *this;
        }

        bool operator==(const ConstIterator& other) const noexcept {
            return m_map == other.m_map && m_index == other.m_index;
        }

        bool operator!=(const ConstIterator& other) const noexcept {
            return !(*this == other);
        }

    private:
        void advanceToOccupied() {
            while (nullptr != m_map && m_index < m_map->m_capacity && !m_map->entries()[m_index].isOccupied()) {
                ++m_index;
            }
        }
    };

    using iterator = Iterator;
    using const_iterator = ConstIterator;

    static Result<Map<Key, Value>> make() {
        return make(kDefaultCapacity);
    }

    static Result<Map<Key, Value>> make(size_t capacity) {
        const size_t bucketCount = normalizeCapacity(capacity);
        GENERIC_CHECK(0 != bucketCount, KERN_INVALID_ARGUMENT, "Map capacity must be greater than zero");
        GENERIC_CHECK(bucketCount <= (SIZE_MAX / sizeof(MapEntry<Key, Value>)), KERN_NO_SPACE, "Map capacity too large");

        CHECK_RESULT(buffer, RawBuffer::make(bucketCount * sizeof(MapEntry<Key, Value>)), "Failed to allocate map buffer");

        MapEntry<Key, Value>* entryBuffer = reinterpret_cast<MapEntry<Key, Value>*>(buffer.getCharBuffer());
        for (size_t index = 0; index < bucketCount; ++index) {
            new (entryBuffer + index) MapEntry<Key, Value>();
        }

        return Result<Map<Key, Value>>::make(Map(move(buffer), 0, bucketCount));
    }

    Map(const Map&) = delete;
    Map& operator=(const Map&) = delete;

    Map(Map&& other) noexcept
        : m_buffer(move(other.m_buffer))
        , m_size(other.m_size)
        , m_capacity(other.m_capacity)
    {
        other.m_size = 0;
        other.m_capacity = 0;
    }

    Map& operator=(Map&& other) {
        if (this == &other) {
            return *this;
        }

        destroyEntries();
        m_buffer = move(other.m_buffer);
        m_size = other.m_size;
        m_capacity = other.m_capacity;

        other.m_size = 0;
        other.m_capacity = 0;

        return *this;
    }

    ~Map() {
        destroyEntries();
    }

    template<typename K, typename V>
    Result<void> insert(K&& key, V&& value) {
        if (shouldRehashBeforeInsert()) {
            CHECK_RESULT_NO_VALUE(rehash(m_capacity * 2), "Failed to rehash map before insert");
        }

        const size_t slotIndex = findInsertSlot(key);
        GENERIC_CHECK(slotIndex < m_capacity, KERN_NO_SPACE, "Map is full");

        MapEntry<Key, Value>& entry = entries()[slotIndex];
        if (entry.isOccupied()) {
            entry.value() = forward<V>(value);
            return Result<void>::make();
        }

        entry.construct(forward<K>(key), forward<V>(value));
        ++m_size;

        return Result<void>::make();
    }

    template<typename K>
    Value* find(K&& key) {
        const Optional<size_t> slotIndex = findOccupiedSlot(key);
        if (!slotIndex.hasValue()) {
            return nullptr;
        }

        return &entries()[slotIndex.value()].value();
    }

    template<typename K>
    const Value* find(K&& key) const {
        const Optional<size_t> slotIndex = findOccupiedSlot(key);
        if (!slotIndex.hasValue()) {
            return nullptr;
        }

        return &entries()[slotIndex.value()].value();
    }

    template<typename K>
    bool contains(K&& key) const {
        return find(forward<K>(key)) != nullptr;
    }

    template<typename K>
    Result<void> remove(K&& key) {
        const Optional<size_t> slotIndex = findOccupiedSlot(key);
        if (!slotIndex.hasValue()) {
            return Result<void>::makeError(KERN_NOT_FOUND);
        }

        entries()[slotIndex.value()].markTombstone();
        --m_size;

        return Result<void>::make();
    }

    Result<void> clear() {
        destroyEntries();
        m_size = 0;
        return Result<void>::make();
    }

    size_t size() const noexcept {
        return m_size;
    }

    size_t capacity() const noexcept {
        return m_capacity;
    }

    bool empty() const noexcept {
        return 0 == m_size;
    }

    iterator begin() noexcept {
        return iterator(this, 0);
    }

    iterator end() noexcept {
        return iterator(this, m_capacity);
    }

    const_iterator begin() const noexcept {
        return const_iterator(this, 0);
    }

    const_iterator end() const noexcept {
        return const_iterator(this, m_capacity);
    }

    const_iterator cbegin() const noexcept {
        return begin();
    }

    const_iterator cend() const noexcept {
        return end();
    }

private:
    static constexpr size_t kDefaultCapacity = 8;

    Map(RawBuffer&& buffer, size_t size, size_t capacity)
        : m_buffer(move(buffer))
        , m_size(size)
        , m_capacity(capacity)
    {}

    MapEntry<Key, Value>* entries() noexcept {
        return reinterpret_cast<MapEntry<Key, Value>*>(m_buffer.getCharBuffer());
    }

    const MapEntry<Key, Value>* entries() const noexcept {
        return reinterpret_cast<const MapEntry<Key, Value>*>(m_buffer.getCharBuffer());
    }

    static size_t normalizeCapacity(size_t capacity) {
        if (0 == capacity) {
            return kDefaultCapacity;
        }

        size_t normalized = 1;
        while (normalized < capacity) {
            if (normalized > (SIZE_MAX / 2)) {
                return SIZE_MAX;
            }

            normalized *= 2;
        }

        return normalized;
    }

    static size_t hashKey(const Key& key) {
        return MapHash<Key>()(key);
    }

    bool shouldRehashBeforeInsert() const {
        if (0 == m_capacity) {
            return true;
        }

        return (m_size + 1) * 4 > m_capacity * 3;
    }

    template<typename K>
    Optional<size_t> findOccupiedSlot(K&& key) const {
        if (0 == m_capacity) {
            return Optional<size_t>();
        }

        size_t index = hashKey(key) & (m_capacity - 1);
        for (size_t probe = 0; probe < m_capacity; ++probe) {
            const MapEntry<Key, Value>& entry = entries()[index];
            if (entry.isEmpty()) {
                return Optional<size_t>();
            }

            if (entry.isOccupied() && entry.key() == key) {
                return Optional<size_t>(index);
            }

            index = (index + 1) & (m_capacity - 1);
        }

        return Optional<size_t>();
    }

    template<typename K>
    size_t findInsertSlot(K&& key) {
        size_t firstTombstone = m_capacity;
        size_t index = hashKey(key) & (m_capacity - 1);

        for (size_t probe = 0; probe < m_capacity; ++probe) {
            MapEntry<Key, Value>& entry = entries()[index];
            if (entry.isEmpty()) {
                return firstTombstone < m_capacity ? firstTombstone : index;
            }

            if (entry.isTombstone() && firstTombstone == m_capacity) {
                firstTombstone = index;
            }

            if (entry.isOccupied() && entry.key() == key) {
                return index;
            }

            index = (index + 1) & (m_capacity - 1);
        }

        return firstTombstone;
    }

    Result<void> rehash(size_t newCapacity) {
        const size_t bucketCount = normalizeCapacity(newCapacity);
        if (bucketCount <= m_capacity) {
            return Result<void>::make();
        }

        CHECK_RESULT(newMap, (Map<Key, Value>::make(bucketCount)), "Failed to allocate rehashed map");

        for (size_t index = 0; index < m_capacity; ++index) {
            MapEntry<Key, Value>& entry = entries()[index];
            if (!entry.isOccupied()) {
                continue;
            }

            CHECK_RESULT_NO_VALUE(
                newMap.insert(move(entry.key()), move(entry.value())),
                "Failed to reinsert map entry during rehash");
            entry.abandon();
        }

        destroyEntries();
        m_buffer = move(newMap.m_buffer);
        m_size = newMap.m_size;
        m_capacity = newMap.m_capacity;

        newMap.m_size = 0;
        newMap.m_capacity = 0;

        return Result<void>::make();
    }

    void destroyEntries() {
        if (0 == m_capacity || nullptr == entries()) {
            return;
        }

        for (size_t index = 0; index < m_capacity; ++index) {
            entries()[index].~MapEntry<Key, Value>();
        }
    }

    RawBuffer m_buffer;
    size_t m_size;
    size_t m_capacity;
};

} // namespace dstd
