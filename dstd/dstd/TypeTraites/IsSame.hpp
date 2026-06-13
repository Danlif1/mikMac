//
//  IsSame.cpp
//  dstd
//
//  Created by Daniel Lifshitz on 01/10/2025.
//
#pragma once

#include "BasicTypeTraites.hpp"


namespace dstd {

template<typename A, typename B>
struct is_same : false_type {};
template<typename A>
struct is_same<A, A> : true_type {};

} // namespace dstd
