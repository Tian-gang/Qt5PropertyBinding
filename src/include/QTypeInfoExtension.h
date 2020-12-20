#pragma once

#include <functional>
#include <optional>
#include <type_traits>
#include <variant>

#define QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_MOVE_AND_SWAP(Class) \
    Class& operator=(Class&& other) noexcept                      \
    {                                                             \
        Class moved(std::move(other));                            \
        swap(moved);                                              \
        return *this;                                             \
    }

#define QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_PURE_SWAP(Class) \
    Class& operator=(Class&& other) noexcept                  \
    {                                                         \
        swap(other);                                          \
        return *this;                                         \
    }

namespace detail
{
// find out whether T is a conteiner
// this is required to check the value type of containers for the existence of the comparison operator
template <typename, typename = void>
struct is_container : std::false_type
{
};
template <typename T>
struct is_container<T, std::void_t<std::is_convertible<decltype(std::declval<T>().begin() != std::declval<T>().end()), bool>,
                                   typename T::value_type>> : std::true_type
{
};

// Checks the existence of the comparison operator for the class itself
template <typename, typename = void>
struct has_operator_equal : std::false_type
{
};
template <typename T>
struct has_operator_equal<T, std::void_t<decltype(bool(std::declval<const T&>() == std::declval<const T&>()))>> : std::true_type
{
};

// Two forward declarations
template <typename T, bool = is_container<T>::value>
struct expand_operator_equal_container;
template <typename T>
struct expand_operator_equal_tuple;

// the entry point for the public method
template <typename T>
using expand_operator_equal = expand_operator_equal_container<T>;

// if T isn't a container check if it's a tuple like object
template <typename T, bool>
struct expand_operator_equal_container : expand_operator_equal_tuple<T>
{
};
// if T::value_type exists, check first T::value_type, then T itself
template <typename T>
struct expand_operator_equal_container<T, true>
    : std::conjunction<expand_operator_equal<typename T::value_type>, expand_operator_equal_tuple<T>>
{
};

// recursively check the template arguments of a tuple like object
template <typename... T>
using expand_operator_equal_recursive = std::conjunction<expand_operator_equal<T>...>;

template <typename T>
struct expand_operator_equal_tuple : has_operator_equal<T>
{
};
template <typename T>
struct expand_operator_equal_tuple<std::optional<T>> : has_operator_equal<T>
{
};
template <typename T1, typename T2>
struct expand_operator_equal_tuple<std::pair<T1, T2>> : expand_operator_equal_recursive<T1, T2>
{
};
template <typename... T>
struct expand_operator_equal_tuple<std::tuple<T...>> : expand_operator_equal_recursive<T...>
{
};
template <typename... T>
struct expand_operator_equal_tuple<std::variant<T...>> : expand_operator_equal_recursive<T...>
{
};

// the same for operator<(), see above for explanations
template <typename, typename = void>
struct has_operator_less_than : std::false_type
{
};
template <typename T>
struct has_operator_less_than<T, std::void_t<decltype(bool(std::declval<const T&>() < std::declval<const T&>()))>>
    : std::true_type
{
};

template <typename T, bool = is_container<T>::value>
struct expand_operator_less_than_container;
template <typename T>
struct expand_operator_less_than_tuple;

template <typename T>
using expand_operator_less_than = expand_operator_less_than_container<T>;

template <typename T, bool>
struct expand_operator_less_than_container : expand_operator_less_than_tuple<T>
{
};
template <typename T>
struct expand_operator_less_than_container<T, true>
    : std::conjunction<expand_operator_less_than<typename T::value_type>, expand_operator_less_than_tuple<T>>
{
};

template <typename... T>
using expand_operator_less_than_recursive = std::conjunction<expand_operator_less_than<T>...>;

template <typename T>
struct expand_operator_less_than_tuple : has_operator_less_than<T>
{
};
template <typename T1, typename T2>
struct expand_operator_less_than_tuple<std::pair<T1, T2>> : expand_operator_less_than_recursive<T1, T2>
{
};
template <typename... T>
struct expand_operator_less_than_tuple<std::tuple<T...>> : expand_operator_less_than_recursive<T...>
{
};
template <typename... T>
struct expand_operator_less_than_tuple<std::variant<T...>> : expand_operator_less_than_recursive<T...>
{
};

}  // namespace detail

namespace QTypeTraits
{
template <typename T, typename = void>
struct is_dereferenceable : std::false_type
{
};

template <typename T>
struct is_dereferenceable<T, std::void_t<decltype(std::declval<T>().operator->())>> : std::true_type
{
};

template <typename T>
inline constexpr bool is_dereferenceable_v = is_dereferenceable<T>::value;

template <typename T>
struct has_operator_equal : detail::expand_operator_equal<T>
{
};
template <typename T>
inline constexpr bool has_operator_equal_v = has_operator_equal<T>::value;
}  // namespace QTypeTraits
