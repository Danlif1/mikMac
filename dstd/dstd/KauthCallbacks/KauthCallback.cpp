//
//  KauthCallback.cpp
//  MyKext
//
//  Created by Daniel Lifshitz on 05/09/2025.
//

#include "KauthCallback.hpp"


namespace dstd {

void KauthCallbackDeleter::operator()(kauth_listener_t* listenerPointer) {
    if (nullptr != listenerPointer && nullptr != *listenerPointer) {
        kauth_unlisten_scope(*listenerPointer);
    }
}

} // namespace dstd
