//
//  Lock.cpp
//  dstd
//
//  Created by Daniel Lifshitz on 19/06/2026.
//
#include "Lock.hpp"

#include "Checkers.hpp"


namespace dstd {

void LockGroupAttrDeleter::operator()(lck_grp_attr_t* attributes) {
    if (nullptr != attributes) {
        lck_grp_attr_free(attributes);
    }
}

void LockAttrDeleter::operator()(lck_attr_t* attributes) {
    if (nullptr != attributes) {
        lck_attr_free(attributes);
    }
}

void LockGroupDeleter::operator()(lck_grp_t* group) {
    if (nullptr != group) {
        lck_grp_free(group);
    }
}

void LockRwDeleter::operator()(lck_rw_t* lock) {
    if (nullptr != lock) {
        lck_rw_free(lock, m_group);
    }
}

Result<Lock> Lock::make(const char* groupName) {
    GENERIC_CHECK(nullptr != groupName, KERN_INVALID_ARGUMENT, "lock group name is null");

    lck_grp_attr_t* rawGroupAttributes = lck_grp_attr_alloc_init();
    GENERIC_CHECK(nullptr != rawGroupAttributes, KERN_NO_SPACE, "failed to allocate lock group attributes");
    UniquePtr<lck_grp_attr_t, LockGroupAttrDeleter> groupAttributes(rawGroupAttributes, LockGroupAttrDeleter());

    lck_grp_t* rawGroup = lck_grp_alloc_init(groupName, groupAttributes.getValue());
    GENERIC_CHECK(nullptr != rawGroup, KERN_NO_SPACE, "failed to allocate lock group");
    UniquePtr<lck_grp_t, LockGroupDeleter> group(rawGroup, LockGroupDeleter());

    lck_attr_t* rawLockAttributes = lck_attr_alloc_init();
    GENERIC_CHECK(nullptr != rawLockAttributes, KERN_NO_SPACE, "failed to allocate lock attributes");

    UniquePtr<lck_attr_t, LockAttrDeleter> lockAttributes(rawLockAttributes, LockAttrDeleter());

    lck_rw_t* rawLock = lck_rw_alloc_init(group.getValue(), lockAttributes.getValue());
    GENERIC_CHECK(nullptr != rawLock, KERN_NO_SPACE, "failed to allocate lock");

    const LockRwDeleter rwDeleter { group.getValue() };
    UniquePtr<lck_rw_t, LockRwDeleter> lock(rawLock, rwDeleter);

    return Lock(move(group), move(lock));
}

Lock::Lock(UniquePtr<lck_grp_t, LockGroupDeleter>&& group, UniquePtr<lck_rw_t, LockRwDeleter>&& lock)
    : m_group(move(group))
    , m_lock(move(lock))
{}

Lock::Lock(Lock&& other) noexcept
    : m_group(move(other.m_group))
    , m_lock(move(other.m_lock))
{}

Lock& Lock::operator=(Lock&& other) {
    if (this == &other) {
        return *this;
    }

    m_group = move(other.m_group);
    m_lock = move(other.m_lock);

    return *this;
}

Lock::~Lock() = default;

lck_rw_t* Lock::rwLock() {
    return m_lock.getValue();
}

void Lock::lockExclusive() {
    lck_rw_lock_exclusive(rwLock());
}

void Lock::unlockExclusive() {
    lck_rw_unlock_exclusive(rwLock());
}

void Lock::lockShared() {
    lck_rw_lock_shared(rwLock());
}

void Lock::unlockShared() {
    lck_rw_unlock_shared(rwLock());
}

} // namespace dstd
