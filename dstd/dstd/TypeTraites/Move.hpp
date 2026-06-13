//
//  Move.hpp
//  dstd
//
//  Created by Daniel Lifshitz on 22/08/2025.
//
#pragma once

#include "BasicTypeTraites.hpp"


namespace dstd {

template<typename T>
remove_reference_t<T>&& move(T&& value) {
    return static_cast<remove_reference_t<T>&&>(value);
}

} // namespace dstd
