//
//  BasicTypeTraites.hpp
//  dstd
//
//  Created by Daniel Lifshitz on 01/10/2025.
//
#pragma once


namespace dstd {

template<typename T, T v>
struct integral_constant {
    static constexpr T value = v;
    using value_type = T;
    using type = integral_constant<T, v>;
    constexpr operator value_type() const noexcept { return value; }
};

using true_type  = integral_constant<bool, true>;
using false_type = integral_constant<bool, false>;

template<typename T>
struct remove_reference { using type = T; };
template<typename T>
struct remove_reference<T&> { using type = T; };
template<typename T>
struct remove_reference<T&&> { using type = T; };

template<typename T>
using remove_reference_t = typename remove_reference<T>::type;

template<typename T> struct is_lvalue_reference : false_type {};
template<typename T> struct is_lvalue_reference<T&> : true_type {};

template<typename T> struct add_rvalue_reference { typedef T && type; };
template<typename T> struct add_rvalue_reference<T&> { typedef T& type; };
template<> struct add_rvalue_reference<void> { typedef void type; };
template<> struct add_rvalue_reference<const void> { typedef const void type; };
template<> struct add_rvalue_reference<volatile void> { typedef volatile void type; };
template<> struct add_rvalue_reference<const volatile void> { typedef const volatile void type; };

template<typename T>
using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;

template<typename T>
add_rvalue_reference_t<T> declval() noexcept;

template<typename T> struct remove_const { using type = T; };
template<typename T> struct remove_const<const T> { using type = T; };

template<typename T> struct remove_volatile { using type = T; };
template<typename T> struct remove_volatile<volatile T> { using type = T; };

template<typename T>
struct remove_cv {
    using type = typename remove_volatile<typename remove_const<T>::type>::type;
};

template<typename T>
using remove_cv_t = typename remove_cv<T>::type;

template<typename T>
struct remove_cvref {
    using type = typename remove_cv<typename remove_reference<T>::type>::type;
};

template<typename T>
using remove_cvref_t = typename remove_cvref<T>::type;

#if defined(__clang__) || defined(__GNUC__)
#   define HAS_INTRINSIC_TRIVIAL_COPYABLE 1
#   define HAS_INTRINSIC_TRIVIAL_CONSTRUCTIBLE 1
#endif

template<typename T>
struct is_trivially_copyable {
#if defined(HAS_INTRINSIC_TRIVIAL_COPYABLE)
    static constexpr bool value = __is_trivially_copyable(T);
#else
    static constexpr bool value = false;
#endif
};

template<typename T, typename... Args>
struct is_constructible {
private:
    template<typename U, typename... Ts,
             typename = decltype(U(declval<Ts>()...))>
    static true_type test(int);

    template<typename...>
    static false_type test(...);

public:
    static constexpr bool value = decltype(test<T, Args...>(0))::value;
};

template<typename T, typename... Args>
struct is_nothrow_constructible {
private:
    template<typename U, typename... Ts,
             typename = decltype(U(declval<Ts>()...))>
    static true_type test(int);

    template<typename...>
    static false_type test(...);

public:
    static constexpr bool value =
        decltype(test<T, Args...>(0))::value &&
        noexcept(T(declval<Args>()...));
};

template<typename T, typename... Args>
struct is_trivially_constructible {
#if defined(HAS_INTRINSIC_TRIVIAL_CONSTRUCTIBLE)
    static constexpr bool value = __is_trivially_constructible(T, Args...);
#else
    static constexpr bool value = false;
#endif
};

template<class T>
struct is_move_constructible : is_constructible<T, add_rvalue_reference_t<T>> {};

template<class T>
struct is_trivially_move_constructible : is_trivially_constructible<T, add_rvalue_reference_t<T>> {};

template<class T>
struct is_nothrow_move_constructible : is_nothrow_constructible<T, add_rvalue_reference_t<T>> {};

template<typename T>
constexpr bool is_trivially_copyable_v = is_trivially_copyable<T>::value;

template<typename T>
constexpr bool is_move_constructible_v = is_move_constructible<T>::value;

template<typename T>
constexpr bool is_trivially_move_constructible_v = is_trivially_move_constructible<T>::value;

template<typename T>
constexpr bool is_nothrow_move_constructible_v = is_nothrow_move_constructible<T>::value;

} // namespace dstd
