#ifndef VLITE_META_HPP_INCLUDED
#define VLITE_META_HPP_INCLUDED

#include <type_traits>

namespace vlite::meta
{

template <typename T, template <typename> typename Expression, typename = std::void_t<>>
struct compiles : std::false_type
{
};

template <typename T, template <typename> typename Expression>
struct compiles<T, Expression, std::void_t<Expression<T>>> : std::true_type
{
};

template <typename T, typename R, template <typename> typename Expression,
          typename = std::void_t<>>
struct compiles_same : std::false_type
{
};

template <typename T, typename R, template <typename> typename Expression>
struct compiles_same<T, R, Expression,
                     std::void_t<std::enable_if_t<std::is_same<R, Expression<T>>::value>>>
  : std::true_type
{
};

template <typename T, template <typename...> typename R,
          template <typename> typename Expression, typename = std::void_t<>>
struct compiles_models : std::false_type
{
};

template <typename T, template <typename...> typename R,
          template <typename> typename Expression>
struct compiles_models<T, R, Expression,
                       std::void_t<std::enable_if_t<R<Expression<T>>::value>>>
  : std::true_type
{
};

template <typename... Checks>
using requires = std::enable_if_t<std::conjunction<Checks...>::value>;

template <typename... Checks>
using fallback = std::enable_if_t<std::conjunction<std::negation<Checks...>>::value>;

template <typename R, typename... Checks>
using requires_t = std::enable_if_t<std::conjunction<Checks...>::value, R>;

template <typename R, typename... Checks>
using fallback_t = std::enable_if_t<std::conjunction<std::negation<Checks>...>::value, R>;

template <typename...> struct always_false : std::false_type
{
};

template <typename...> struct always_true : std::true_type
{
};

} // namespace vlite::meta

#endif // VLITE_META_HPP_INCLUDED
