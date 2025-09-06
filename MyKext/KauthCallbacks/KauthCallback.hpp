//
//  KauthCallback.hpp
//  MyKext
//
//  Created by Daniel Lifshitz on 05/09/2025.
//
#pragma once

#include <sys/kauth.h>

#include "../Checkers.hpp"
#include "../Logger.h"
#include "../Result.hpp"

#include "../Pointers/UniquePtr.hpp"


namespace dstd {

struct KauthCallbackDeleter
{
    void operator()(kauth_listener_t* listenerPointer);
};

template<typename T>
class KauthCallback
{
public:
    static Result<KauthCallback<T>> make(const char* identifier, kauth_scope_callback_t callback, T&& data) {
        GENERIC_CHECK(nullptr != identifier, KERN_INVALID_ADDRESS, "Invalid identifier");
        
        CHECK_RESULT(context, UniquePtr<T>::make(data), "Failed to create context");
        KauthCallback<T> result(move(context));
        
        auto rawListener = kauth_listen_scope(identifier, callback, result.m_data.getValue());
        GENERIC_CHECK(nullptr != rawListener, KERN_FAILURE, "Failed to listen scope");
        
        auto listenerResult = UniquePtr<kauth_listener_t, KauthCallbackDeleter>::make(rawListener);
        if (listenerResult.hasError()) {
            KauthCallbackDeleter()(&rawListener);
            return dstd::Result<void>::makeError(KERN_FAILURE);
        }
        
        result.m_listener = move(listenerResult.value());
        
        return Result<KauthCallback<T>>::make(move(result));
    }
    
    bool operator==(const KauthCallback<T>& other) const {
        return m_data == other.m_data && m_listener == other.m_listener;
    }

    bool isValid() {
        return m_listener.hasValue();
    }
    
    T* getData() {
        return &m_data.getValue();
    }
    
private:
    KauthCallback(UniquePtr<kauth_listener_t, KauthCallbackDeleter>&& listener, UniquePtr<T>&& data)
        : m_listener(move(listener))
        , m_data(move(data))
    {}
    
    KauthCallback(UniquePtr<T>&& data)
        : m_listener()
        , m_data(move(data))
    {}
    
    Optional<UniquePtr<kauth_listener_t, KauthCallbackDeleter>> m_listener;
    UniquePtr<T> m_data;
};

template<>
class KauthCallback<void> {
public:
    static Result<KauthCallback<void>> make(const char* identifier, kauth_scope_callback_t callback) {
        GENERIC_CHECK(nullptr != identifier, KERN_INVALID_ADDRESS, "Invalid identifier");

        KauthCallback<void> result;
        auto rawListener = kauth_listen_scope(identifier, callback, nullptr);
        GENERIC_CHECK(nullptr != rawListener, KERN_FAILURE, "Failed to listen to scope");
        
        auto listenerResult = UniquePtr<kauth_listener_t, KauthCallbackDeleter>::make(rawListener);
        if (listenerResult.hasError()) {
            KauthCallbackDeleter()(&rawListener);
            return dstd::Result<void>::makeError(KERN_FAILURE);
        }
        
        result.m_listener = move(listenerResult.value());
        
        return Result<KauthCallback<void>>::make(move(result));
    }
    
    bool operator==(const KauthCallback<void>& other) const {
        return m_listener == other.m_listener;
    }

    bool isValid() {
        return m_listener.hasValue();
    }
    
    void* getData() {
        return nullptr;
    }
    
private:
    KauthCallback(UniquePtr<kauth_listener_t, KauthCallbackDeleter>&& listener)
        : m_listener(move(listener))
    {}
    
    KauthCallback()
        : m_listener()
    {}
    
    Optional<UniquePtr<kauth_listener_t, KauthCallbackDeleter>> m_listener;
};

} // namespace dstd
