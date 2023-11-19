#pragma once

#include <type_traits>
#include <memory>

namespace eh {

namespace utils {

template <typename T, typename = bool>
struct is_equality_comparable : std::false_type {};

template <typename T>
struct is_equality_comparable<T, decltype(std::declval<T&>() ==
                                          std::declval<T&>())>
    : std::true_type {};

template <typename T>
inline constexpr bool is_equality_comparable_v =
    is_equality_comparable<T>::value;


template <typename T>
struct is_shared_ptr : std::false_type {};

template <typename T>
struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};

template <typename T>
inline constexpr bool is_shared_ptr_v = is_shared_ptr<T>::value;


template <typename T>
struct is_weak_ptr : std::false_type {};

template <typename T>
struct is_weak_ptr<std::weak_ptr<T>> : std::true_type {};

template <typename T>
inline constexpr bool is_weak_ptr_v = is_weak_ptr<T>::value;

template <typename T>
inline constexpr bool is_shared_or_weak_ptr_v =
    is_shared_ptr_v<T> | is_weak_ptr_v<T>; 


}  // namespace detail

}  // namespace eh
