//
//  Lock.hpp
//  dstd
//
//  Created by Daniel Lifshitz on 19/06/2026.
//
#pragma once

#include <libkern/locks.h>

#include "Pointers/UniquePtr.hpp"
#include "Result.hpp"


namespace dstd {

struct LockGroupAttrDeleter {
    void operator()(lck_grp_attr_t* attributes);
};

struct LockAttrDeleter {
    void operator()(lck_attr_t* attributes);
};

struct LockGroupDeleter {
    void operator()(lck_grp_t* group);
};

struct LockRwDeleter {
    lck_grp_t* m_group;

    void operator()(lck_rw_t* lock);
};

class Lock {
public:
    static Result<Lock> make(const char* groupName = "dstd.lock");

    Lock(const Lock&) = delete;
    Lock& operator=(const Lock&) = delete;

    Lock(Lock&& other) noexcept;
    Lock& operator=(Lock&& other);

    ~Lock();

    void lockExclusive();
    void unlockExclusive();
    void lockShared();
    void unlockShared();

private:
    Lock(UniquePtr<lck_grp_t, LockGroupDeleter>&& group, UniquePtr<lck_rw_t, LockRwDeleter>&& lock);

    lck_rw_t* rwLock();

    UniquePtr<lck_grp_t, LockGroupDeleter> m_group;
    UniquePtr<lck_rw_t, LockRwDeleter> m_lock;
};

} // namespace dstd
