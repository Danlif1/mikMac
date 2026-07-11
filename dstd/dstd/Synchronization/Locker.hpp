//
//  Locker.hpp
//  dstd
//
//  Created by Daniel Lifshitz on 19/06/2026.
//
#pragma once

#include "Checkers.hpp"
#include "Result.hpp"
#include "Synchronization/Lock.hpp"
#include "TypeTraites/TypeTraites.hpp"


namespace dstd {

template<typename T>
class Locker {
public:
    class ExclusiveGuard {
    public:
        explicit ExclusiveGuard(Locker& locker)
            : m_locker(&locker)
        {
            m_locker->m_lock.lockExclusive();
        }

        ExclusiveGuard(const ExclusiveGuard&) = delete;
        ExclusiveGuard& operator=(const ExclusiveGuard&) = delete;

        ExclusiveGuard(ExclusiveGuard&& other) noexcept
            : m_locker(other.m_locker)
        {
            other.m_locker = nullptr;
        }

        ExclusiveGuard& operator=(ExclusiveGuard&& other) noexcept {
            if (this == &other) {
                return *this;
            }

            if (nullptr != m_locker) {
                m_locker->m_lock.unlockExclusive();
            }

            m_locker = other.m_locker;
            other.m_locker = nullptr;

            return *this;
        }

        ~ExclusiveGuard() {
            if (nullptr != m_locker) {
                m_locker->m_lock.unlockExclusive();
            }
        }

        T& get() {
            return m_locker->m_object;
        }

        T* operator->() {
            return &m_locker->m_object;
        }

    private:
        Locker* m_locker;
    };

    class SharedGuard {
    public:
        explicit SharedGuard(Locker& locker)
            : m_locker(&locker)
        {
            m_locker->m_lock.lockShared();
        }

        SharedGuard(const SharedGuard&) = delete;
        SharedGuard& operator=(const SharedGuard&) = delete;

        SharedGuard(SharedGuard&& other) noexcept
            : m_locker(other.m_locker)
        {
            other.m_locker = nullptr;
        }

        SharedGuard& operator=(SharedGuard&& other) noexcept {
            if (this == &other) {
                return *this;
            }

            if (nullptr != m_locker) {
                m_locker->m_lock.unlockShared();
            }

            m_locker = other.m_locker;
            other.m_locker = nullptr;

            return *this;
        }

        ~SharedGuard() {
            if (nullptr != m_locker) {
                m_locker->m_lock.unlockShared();
            }
        }

        const T& get() const {
            return m_locker->m_object;
        }

        const T* operator->() const {
            return &m_locker->m_object;
        }

    private:
        Locker* m_locker;
    };

    static Result<Locker<T>> make(T&& object, const char* lockName = "dstd.locker") {
        CHECK_RESULT(lock, Lock::make(lockName), "Failed to create locker lock");
        return Locker(move(lock), move(object));
    }

    Locker(const Locker&) = delete;
    Locker& operator=(const Locker&) = delete;

    Locker(Locker&& other) noexcept
        : m_lock(move(other.m_lock))
        , m_object(move(other.m_object))
    {}

    Locker& operator=(Locker&& other) noexcept {
        if (this == &other) {
            return *this;
        }

        m_lock = move(other.m_lock);
        m_object = move(other.m_object);

        return *this;
    }

    ExclusiveGuard lockExclusive() {
        return ExclusiveGuard(*this);
    }

    SharedGuard lockShared() const {
        return SharedGuard(const_cast<Locker&>(*this));
    }

private:
    Locker(Lock&& lock, T&& object)
        : m_lock(move(lock))
        , m_object(move(object))
    {}

    Lock m_lock;
    T m_object;
};

} // namespace dstd
