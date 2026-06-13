//
//  Basics.hpp
//  dstd
//
//  Created by Daniel Lifshitz on 05/12/2025.
//
#pragma once

#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

namespace dstd {

template<typename T>
T max(T first, T second) {
    return first > second ? first : second;
}

template<typename T>
T min(T first, T second) {
    return first < second ? first : second;
}

} // namespace dstd
