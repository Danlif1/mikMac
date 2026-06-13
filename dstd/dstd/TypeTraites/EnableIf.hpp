//
//  EnableIf.hpp
//  dstd
//
//  Created by Daniel Lifshitz on 01/10/2025.
//
#pragma once

#include "BasicTypeTraites.hpp"


namespace dstd {

template<bool B, typename T = void>
struct enable_if { };

template<typename T>
struct enable_if<true, T> { using type = T; };

template<bool B, typename T = void>
using enable_if_t = typename enable_if<B, T>::type;

} // namespace dstd
