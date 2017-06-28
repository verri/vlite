#ifndef VLITE_NUMERIC_HPP_INCLUDED
#define VLITE_NUMERIC_HPP_INCLUDED

#include <vlite/builder.hpp>
#include <vlite/common_vector_base.hpp>

namespace vlite
{

template <typename> class vector;

namespace detail
{

template <typename T> struct cat_value_type
{
  using type = std::decay_t<T>;
};

template <typename Vector> struct cat_value_type<common_vector_base<Vector>>
{
  using type = typename Vector::value_type;
};

template <typename T> using cat_value_type_t = typename cat_value_type<T>::type;

template <typename Vector>
constexpr auto cat_size(const common_vector_base<Vector>& vec) noexcept -> std::size_t
{
  return vec.size();
}

template <typename T> constexpr auto cat_size(const T&) noexcept -> std::size_t
{
  return 1u;
}

template <typename T, typename Vector>
auto cat_push(builder<T>& b, const common_vector_base<Vector>& vec)
{
  std::copy(vec.begin(), vec.end(), std::back_inserter(b));
}

template <typename T, typename U> auto cat_push(builder<T>& builder, U&& value)
{
  builder.push_back(std::forward<U>(value));
}

} // namespace detail

template <typename... Args> auto cat(Args&&... args)
{
  using R = std::common_type_t<detail::cat_value_type_t<Args>...>;
  const auto size = (std::size_t{} + ... + detail::cat_size(args));

  auto b = builder<R>(size);

  (detail::cat_push(b, std::forward<Args>(args)), ...);

  return vector<R>(std::move(b));
}

} // namespace vlite

#endif // VLITE_NUMERIC_HPP_INCLUDED
