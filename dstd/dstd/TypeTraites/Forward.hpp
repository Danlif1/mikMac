//
//  Forward.hpp
//  dstd
//
//  Created by Daniel Lifshitz on 01/10/2025.
//
#pragma once

#include "BasicTypeTraites.hpp"


namespace dstd {

template<typename T>
constexpr T&& forward(remove_reference_t<T>& value) noexcept {
    return static_cast<T&&>(value);
}

template<typename T>
constexpr T&& forward(remove_reference_t<T>&& value) noexcept {
    return static_cast<T&&>(value);
}

} // namespace dstd
