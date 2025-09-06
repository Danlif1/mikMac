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

template<typename T = void*, typename Deleter = DefaultDeleter<T>>
class KauthCallback
{
public:
    static Result<KauthCallback<T, Deleter>> make(const char* identifier, kauth_scope_callback_t callback, Optional<UniquePtr<T, Deleter>>&& data) {
        GENERIC_CHECK(nullptr != identifier, KERN_INVALID_ADDRESS);
        
        KauthCallback<T, Deleter> result(move(data));
        
        auto rawListener = kauth_listen_scope(identifier, callback, result.getData());
        GENERIC_CHECK(nullptr != rawListener, KERN_FAILURE);
        
        auto listenerResult = UniquePtr<kauth_listener_t, KauthCallbackDeleter>::make(rawListener);
        if (listenerResult.hasError()) {
            KauthCallbackDeleter()(&rawListener);
            return dstd::Result<void>::makeError(KERN_FAILURE);
        }
        
        UniquePtr<kauth_listener_t, KauthCallbackDeleter>& value = listenerResult.value();
        result.m_listener = move(value);
        
        return Result<KauthCallback<T, Deleter>>::make(move(result));
    }
    
    bool operator==(const KauthCallback<T, Deleter>& other) const {
        return m_data == other.m_data && m_listener == other.m_listener;
    }

    bool isValid() {
        return m_listener.hasValue();
    }
    
    T* getData() {
        return m_data.value().getValue();
    }
    
private:
    KauthCallback(UniquePtr<kauth_listener_t, KauthCallbackDeleter>&& listener, UniquePtr<T, Deleter>&& data)
        : m_listener(move(listener))
        , m_data(move(data))
    {}
    
    KauthCallback(Optional<UniquePtr<T, Deleter>>&& data)
        : m_listener()
        , m_data(move(data))
    {}
    
    Optional<UniquePtr<kauth_listener_t, KauthCallbackDeleter>> m_listener;
    Optional<UniquePtr<T, Deleter>> m_data;
};

} // namespace dstd
