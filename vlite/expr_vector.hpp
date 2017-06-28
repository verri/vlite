#ifndef VLITE_EXPR_VECTOR_HPP_INCLUDED
#define VLITE_EXPR_VECTOR_HPP_INCLUDED

#include <cstddef>
#include <utility>

namespace vlite
{

template <typename Op, std::size_t N> class expr_vector
{
public:
  static constexpr auto order = N;

  using size_type = std::size_t;

  using difference_type = std::ptrdiff_t;

  constexpr expr_vector(Op op, std::size_t size)
    : op_{std::move(op)}
    , size_{size}
  {
  }

  constexpr expr_vector(const expr_vector& source) = delete;
  constexpr expr_vector(expr_vector&& source) noexcept = delete;

  constexpr auto operator=(const expr_vector& source) -> expr_vector& = delete;
  constexpr auto operator=(expr_vector&& source) noexcept -> expr_vector& = delete;

  constexpr auto size() const noexcept { return size_; }

protected:
  template <typename... Args> auto operate(Args&&... args) const
  {
    return op_(std::forward<Args>(args)...);
  }

private:
  Op op_;
  std::size_t size_;
};

} // namespace vlite

#endif // VLITE_EXPR_VECTOR_HPP_INCLUDED
