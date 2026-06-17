//
//  Singleton.hpp
//  dstd
//
//  Created by Daniel Lifshitz on 13/06/2026.
//
#pragma once

#include <sys/errno.h>

#include "Optional.hpp"
#include "Result.hpp"
#include "TypeTraites/TypeTraites.hpp"


namespace dstd {

class SingletonGuard {
public:
    using DestroyFunction = void (*)();

    explicit SingletonGuard(DestroyFunction destroyFunction)
        : m_destroyFunction(destroyFunction)
    {}

    ~SingletonGuard() {
        destroyIfActive();
    }

    SingletonGuard(SingletonGuard&& other)
        : m_destroyFunction(other.m_destroyFunction)
    {
        other.m_destroyFunction = nullptr;
    }

    SingletonGuard& operator=(SingletonGuard&& other) {
        if (this == &other) {
            return *this;
        }

        destroyIfActive();
        m_destroyFunction = other.m_destroyFunction;
        other.m_destroyFunction = nullptr;

        return *this;
    }

    SingletonGuard(const SingletonGuard&) = delete;
    SingletonGuard& operator=(const SingletonGuard&) = delete;

private:
    void destroyIfActive() {
        if (nullptr != m_destroyFunction) {
            m_destroyFunction();
            m_destroyFunction = nullptr;
        }
    }

    DestroyFunction m_destroyFunction;
};

template<typename T>
class Singleton {
public:
    template<typename... Args>
    static Result<SingletonGuard> make(Args&&... args) {
        Optional<T>& instance = storage();
        if (instance.hasValue()) {
            return Error(static_cast<kern_return_t>(EEXIST));
        }

        CHECK_RESULT_NO_VALUE_NO_LOG(constructInstance(forward<Args>(args)...), "Failed to construct singleton instance");

        return SingletonGuard(&destroyInstance);
    }

    static Result<Singleton<T>> get() {
        Optional<T>& instance = storage();
        if (!instance.hasValue()) {
            return Error(static_cast<kern_return_t>(ENOENT));
        }

        return Singleton<T>(instance.value());
    }

    T* operator->() {
        return m_object;
    }

    const T* operator->() const {
        return m_object;
    }

    T& operator*() {
        return *m_object;
    }

    const T& operator*() const {
        return *m_object;
    }

private:
    explicit Singleton(T& object)
        : m_object(&object)
    {}

    static void destroyInstance() {
        storage().reset();
    }

    template<typename... Args>
    static Result<void> constructInstance(Args&&... args) {
        if constexpr (Makeable<T, remove_cvref_t<Args>...>) {
            auto elementResult = T::make(forward<Args>(args)...);
            if (elementResult.hasError()) {
                return Error(elementResult.error());
            }

            storage() = Optional<T>(move(elementResult.value()));
            return {};
        } else {
            storage() = Optional<T>(T(forward<Args>(args)...));
            return {};
        }
    }

    static Optional<T>& storage() {
        static Optional<T>* instance = nullptr;
        if (nullptr == instance) {
            instance = new Optional<T>();
        }

        return *instance;
    }

    T* m_object;
};

} // namespace dstd
