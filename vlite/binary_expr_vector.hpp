#ifndef VLITE_BINARY_EXPR_VECTOR_HPP_INCLUDED
#define VLITE_BINARY_EXPR_VECTOR_HPP_INCLUDED

#include <vlite/common_vector_base.hpp>
#include <vlite/expr_vector.hpp>

#include <iterator>

namespace vlite
{

template <typename LhsIt, typename RhsIt, typename Op>
class binary_expr_vector : public expr_vector<Op>,
                           public common_vector_base<binary_expr_vector<LhsIt, RhsIt, Op>>
{
  using lhs_result = decltype(*std::declval<LhsIt>());
  using rhs_result = decltype(*std::declval<RhsIt>());

public:
  using value_type = std::decay_t<std::result_of_t<Op(lhs_result, rhs_result)>>;

  using typename expr_vector<Op>::size_type;

  using typename expr_vector<Op>::difference_type;

  class iterator
  {
    friend class binary_expr_vector<LhsIt, RhsIt, Op>;

  public:
    using iterator_category = std::input_iterator_tag;
    using value_type = std::decay_t<std::result_of_t<Op(lhs_result, rhs_result)>>;
    using difference_type = std::ptrdiff_t;
    using reference = void;
    using pointer = void;

    constexpr iterator() = default;

    constexpr iterator(const iterator& source) = default;
    constexpr iterator(iterator&& source) noexcept = default;

    constexpr iterator& operator=(const iterator& source) = default;
    constexpr iterator& operator=(iterator&& source) noexcept = default;

    constexpr auto operator++() -> iterator&
    {
      ++lhs_;
      ++rhs_;
      return *this;
    }

    constexpr auto operator++(int) -> iterator
    {
      auto copy = *this;
      ++(*this);
      return copy;
    }

    constexpr auto operator==(const iterator& other) const
    {
      return lhs_ == other.lhs_ && rhs_ == other.rhs_;
    }

    constexpr auto operator!=(const iterator& other) const { return !(*this == other); }

    constexpr auto operator*() const -> value_type
    {
      return source_->operate(*lhs_, *rhs_);
    }

  private:
    constexpr iterator(LhsIt lhs, RhsIt rhs, const binary_expr_vector* source)
      : lhs_{lhs}
      , rhs_{rhs}
      , source_{source}
    {
    }

    LhsIt lhs_;
    RhsIt rhs_;
    const binary_expr_vector* source_;
  };

  using const_iterator = iterator;

public:
  binary_expr_vector(LhsIt lhs_first, LhsIt lhs_last, RhsIt rhs_first, RhsIt rhs_last,
                     const Op& op, std::size_t size)
    : expr_vector<Op>(std::move(op), size)
    , lhs_first_{lhs_first}
    , lhs_last_{lhs_last}
    , rhs_first_{rhs_first}
    , rhs_last_{rhs_last}
  {
  }

  binary_expr_vector(const binary_expr_vector& source) = delete;
  binary_expr_vector(binary_expr_vector&& source) noexcept = delete;

  auto operator=(const binary_expr_vector& source) -> binary_expr_vector& = delete;
  auto operator=(binary_expr_vector&& source) noexcept -> binary_expr_vector& = delete;

  auto begin() const -> const_iterator { return cbegin(); }
  auto end() const -> const_iterator { return cend(); }

  auto cbegin() const -> const_iterator { return {lhs_first_, rhs_first_, this}; }
  auto cend() const -> const_iterator { return {lhs_last_, rhs_last_, this}; }

  using expr_vector<Op>::size;

private:
  LhsIt lhs_first_, lhs_last_;
  RhsIt rhs_first_, rhs_last_;
};

template <typename LhsIt, typename RhsIt, typename Op>
binary_expr_vector(LhsIt, LhsIt, RhsIt, RhsIt, Op, std::size_t)
  ->binary_expr_vector<LhsIt, RhsIt, Op>;

} // namespace vlite

#endif // VLITE_BINARY_EXPR_VECTOR_HPP_INCLUDED
