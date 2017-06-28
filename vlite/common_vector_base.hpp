#ifndef VLITE_COMMON_VECTOR_BASE_HPP_INCLUDED
#define VLITE_COMMON_VECTOR_BASE_HPP_INCLUDED

#include <vlite/meta.hpp>

namespace vlite
{

template <typename Derived> class common_vector_base
{
public:
  auto size() const noexcept { return self().size(); }

  auto begin() noexcept { return self().begin(); }

  auto end() noexcept { return self().end(); }

  auto begin() const noexcept { return self().begin(); }

  auto end() const noexcept { return self().end(); }

  auto cbegin() const noexcept { return self().cbegin(); }

  auto cend() const noexcept { return self().cend(); }

protected:
  constexpr common_vector_base() noexcept = default;

private:
  constexpr auto self() const noexcept -> const Derived&
  {
    return static_cast<const Derived&>(*this);
  }

  constexpr auto self() noexcept -> Derived& { return static_cast<Derived&>(*this); }
};

template <typename T, typename = void> struct CommonVector : std::false_type
{
};

template <typename T>
struct CommonVector<T, meta::requires<std::is_base_of<common_vector_base<T>, T>>>
  : std::true_type
{
};

} // namespace vlite

#endif // VLITE_COMMON_VECTOR_BASE_HPP_INCLUDED
