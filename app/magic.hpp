/**
  * Inspired by: https://habrahabr.ru/users/Unrul/
  *
  */

#pragma once

#include <type_traits>

namespace magic {

struct sink { template<typename ...Args> sink(Args const & ... ) {} };

template <typename T, typename ...Args>
constexpr T get_by_type_impl(T head, Args ...tail)
{
    sink{tail...};
    return head;
}

template <typename T, typename ...Args>
constexpr T get_by_type_impl()
{
    return T();
}

template <typename T, typename H, typename ...Args>
constexpr typename std::enable_if<!std::is_same<T, H>::value, T>::type get_by_type_impl(H head, Args ...tail)
{
    (void)head;
    return get_by_type_impl<T>(tail...);
}

template <typename T, typename... Args> constexpr T get(Args... tail)
{
    return get_by_type_impl<T>(tail...);
}

template <typename T>
struct Argument
{
    explicit Argument(const T value) : m_v{value} {}
    T m_v;
    operator T() const { return m_v; }
};

} // namespace magic
