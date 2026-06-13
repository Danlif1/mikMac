//
//  Tuple.hpp
//  dstd
//
//  Created by Daniel Lifshitz on 21/12/2025.
//
#pragma once

#include <stddef.h>

#include "TypeTraites/TypeTraites.hpp"


namespace dstd {

template<typename... Types>
class Tuple;

template<size_t... Is>
struct index_sequence {
    using type = index_sequence<Is...>;
};

template<size_t N, size_t... Is>
struct make_index_sequence_impl {
    using type = typename make_index_sequence_impl<N - 1, N - 1, Is...>::type;
};

template<size_t... Is>
struct make_index_sequence_impl<0, Is...> {
    using type = index_sequence<Is...>;
};

template<size_t N>
using make_index_sequence = typename make_index_sequence_impl<N>::type;

template<typename T>
struct tuple_size;

template<typename... Types>
struct tuple_size<Tuple<Types...>> : integral_constant<size_t, sizeof...(Types)> {};

template<typename T>
constexpr size_t tuple_size_v = tuple_size<T>::value;

template<size_t I, typename T>
struct tuple_element;

template<size_t I, typename Head, typename... Tail>
struct tuple_element<I, Tuple<Head, Tail...>> {
    using type = typename tuple_element<I - 1, Tuple<Tail...>>::type;
};

template<typename Head, typename... Tail>
struct tuple_element<0, Tuple<Head, Tail...>> {
    using type = Head;
};

template<size_t I, typename T>
using tuple_element_t = typename tuple_element<I, T>::type;

template<size_t I, typename T>
struct TupleLeaf {
    T value;

    TupleLeaf() = default;

    template<typename U>
    explicit TupleLeaf(U&& element)
        : value(forward<U>(element))
    {}
};

template<typename IndexSequence, typename... Types>
struct TupleImpl;

template<size_t... Is, typename... Types>
struct TupleImpl<index_sequence<Is...>, Types...> : TupleLeaf<Is, Types>... {
    TupleImpl() = default;

    explicit TupleImpl(Types... elements)
        : TupleLeaf<Is, Types>(forward<Types>(elements))...
    {}
};

template<typename... Types>
class Tuple : public TupleImpl<make_index_sequence<sizeof...(Types)>, Types...> {
public:
    using Base = TupleImpl<make_index_sequence<sizeof...(Types)>, Types...>;

    Tuple() = default;

    explicit Tuple(Types... elements)
        : Base(forward<Types>(elements)...)
    {}

    Tuple(const Tuple&) = default;
    Tuple(Tuple&&) = default;
    Tuple& operator=(const Tuple&) = default;
    Tuple& operator=(Tuple&&) = default;
};

template<>
class Tuple<> {
public:
    Tuple() = default;
    Tuple(const Tuple&) = default;
    Tuple(Tuple&&) = default;
    Tuple& operator=(const Tuple&) = default;
    Tuple& operator=(Tuple&&) = default;
};

template<>
struct tuple_size<Tuple<>> : integral_constant<size_t, 0> {};

template<size_t I, typename T>
T& getTupleLeaf(TupleLeaf<I, T>& leaf) {
    return leaf.value;
}

template<size_t I, typename T>
const T& getTupleLeaf(const TupleLeaf<I, T>& leaf) {
    return leaf.value;
}

template<size_t I, typename T>
T&& getTupleLeaf(TupleLeaf<I, T>&& leaf) {
    return move(leaf.value);
}

template<size_t I, typename T>
const T&& getTupleLeaf(const TupleLeaf<I, T>&& leaf) {
    return move(leaf.value);
}

template<size_t I, typename... Types>
tuple_element_t<I, Tuple<Types...>>& get(Tuple<Types...>& tuple) {
    static_assert(I < sizeof...(Types), "Tuple index out of bounds");
    return getTupleLeaf<I>(static_cast<TupleLeaf<I, tuple_element_t<I, Tuple<Types...>>>&>(tuple));
}

template<size_t I, typename... Types>
const tuple_element_t<I, Tuple<Types...>>& get(const Tuple<Types...>& tuple) {
    static_assert(I < sizeof...(Types), "Tuple index out of bounds");
    return getTupleLeaf<I>(static_cast<const TupleLeaf<I, tuple_element_t<I, Tuple<Types...>>>&>(tuple));
}

template<size_t I, typename... Types>
tuple_element_t<I, Tuple<Types...>>&& get(Tuple<Types...>&& tuple) {
    static_assert(I < sizeof...(Types), "Tuple index out of bounds");
    return getTupleLeaf<I>(static_cast<TupleLeaf<I, tuple_element_t<I, Tuple<Types...>>>&&>(move(tuple)));
}

template<size_t I, typename... Types>
const tuple_element_t<I, Tuple<Types...>>&& get(const Tuple<Types...>&& tuple) {
    static_assert(I < sizeof...(Types), "Tuple index out of bounds");
    return getTupleLeaf<I>(static_cast<const TupleLeaf<I, tuple_element_t<I, Tuple<Types...>>>&&>(move(tuple)));
}

} // namespace dstd
