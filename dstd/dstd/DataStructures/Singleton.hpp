//
//  Singleton.hpp
//  dstd
//
//  Created by Daniel Lifshitz on 13/06/2026.
//
#pragma once

#include <sys/errno.h>

#include "ISingletonGuard.hpp"
#include "Optional.hpp"
#include "Pointers/UniquePtr.hpp"
#include "Result.hpp"
#include "TypeTraites/TypeTraites.hpp"


namespace dstd {

template<typename T>
class Singleton;

template<typename T>
class SingletonGuard : public ISingletonGuard {
public:
    ~SingletonGuard() override {
        if (m_ownsLifetime) {
            Singleton<T>::destroyInstance();
        }
    }

    SingletonGuard(const SingletonGuard&) = delete;
    SingletonGuard& operator=(const SingletonGuard&) = delete;

    SingletonGuard(SingletonGuard&& other)
        : m_ownsLifetime(other.m_ownsLifetime)
    {
        other.m_ownsLifetime = false;
    }

    SingletonGuard& operator=(SingletonGuard&& other) {
        if (this == &other) {
            return *this;
        }

        if (m_ownsLifetime) {
            Singleton<T>::destroyInstance();
        }

        m_ownsLifetime = other.m_ownsLifetime;
        other.m_ownsLifetime = false;

        return *this;
    }

private:
    explicit SingletonGuard(bool ownsLifetime)
        : m_ownsLifetime(ownsLifetime)
    {}

    friend class Singleton<T>;

    bool m_ownsLifetime;
};

template<typename T>
class Singleton {
public:
    template<typename... Args>
    static Result<SingletonGuard<T>> make(Args&&... args) {
        Optional<T>& instance = storage();
        if (instance.hasValue()) {
            return Result<SingletonGuard<T>>::makeError(static_cast<kern_return_t>(EEXIST));
        }

        CHECK_RESULT_NO_VALUE(constructInstance(forward<Args>(args)...), "Failed to construct singleton instance");

        return Result<SingletonGuard<T>>::make(SingletonGuard<T>(true));
    }

    static Result<UniquePtr<ISingletonGuard>> adoptGuard(SingletonGuard<T>&& guard) {
        return Result<UniquePtr<ISingletonGuard>>::make(
            UniquePtr<ISingletonGuard>(new SingletonGuard<T>(move(guard)))
        );
    }

    static Result<Singleton<T>> get() {
        Optional<T>& instance = storage();
        if (!instance.hasValue()) {
            return Result<Singleton<T>>::makeError(static_cast<kern_return_t>(ENOENT));
        }

        return Result<Singleton<T>>::make(Singleton<T>(instance.value()));
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
        return constructInstance(
            integral_constant<bool, Makeable<T, remove_cvref_t<Args>...>::value>{},
            forward<Args>(args)...
        );
    }

    template<typename... Args>
    static Result<void> constructInstance(integral_constant<bool, true>, Args&&... args) {
        auto elementResult = T::make(forward<Args>(args)...);
        if (elementResult.hasError()) {
            return Result<void>::makeError(elementResult.error());
        }

        storage() = Optional<T>(move(elementResult.value()));
        return Result<void>::make();
    }

    template<typename... Args>
    static Result<void> constructInstance(integral_constant<bool, false>, Args&&... args) {
        storage() = Optional<T>(T(forward<Args>(args)...));
        return Result<void>::make();
    }

    static Optional<T>& storage() {
        static Optional<T>* instance = nullptr;
        if (nullptr == instance) {
            instance = new Optional<T>();
        }

        return *instance;
    }

    friend class SingletonGuard<T>;

    T* m_object;
};

} // namespace dstd
