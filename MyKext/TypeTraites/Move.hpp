//
//  Move.hpp
//  MyKext
//
//  Created by Daniel Lifshitz on 22/08/2025.
//

#ifndef Move_hpp
#define Move_hpp


namespace dstd {

// Simplified std::remove_reference implementation
template <typename T>
struct remove_reference {
    using type = T;
};

// Specialization for lvalue references (e.g., int&)
template <typename T>
struct remove_reference<T&> {
    using type = T;
};

// Specialization for rvalue references (e.g., int&&)
template <typename T>
struct remove_reference<T&&> {
    using type = T;
};

// Simplified std::move implementation
template <typename T>
typename remove_reference<T>::type&& move(T&& arg) {
    // This function casts its argument into an rvalue reference,
    // signaling that the object can be moved from.
    return static_cast<typename remove_reference<T>::type&&>(arg);
}

} // namespace dstd

#endif /* Move_hpp */
