//
//  DefaultMakeable.hpp
//  dstd
//
//  Created by Daniel Lifshitz on 13/06/2026.
//
#pragma once

#include "BasicTypeTraites.hpp"
#include "EnableIf.hpp"
#include "IsSame.hpp"


namespace dstd {

template<typename T>
class Result;

template<typename T>
struct DefaultMakeable {
private:
    template<typename U>
    static auto test(int) -> typename enable_if<
        is_same<Result<U>, remove_cvref_t<decltype(U::make())>>::value,
        true_type
    >::type;

    template<typename>
    static false_type test(...);

public:
    static constexpr bool value = decltype(test<T>(0))::value;
};

template<typename T>
constexpr bool DefaultMakeable_v = DefaultMakeable<T>::value;

template<typename T, typename... Args>
concept Makeable = is_same<
    Result<T>,
    remove_cvref_t<decltype(T::make(declval<Args>()...))>
>::value;

template<typename T, typename... Args>
constexpr bool MakeableV = Makeable<T, Args...>;

} // namespace dstd
