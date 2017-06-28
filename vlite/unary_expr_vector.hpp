#ifndef VLITE_UNARY_EXPR_VECTOR_HPP_INCLUDED
#define VLITE_UNARY_EXPR_VECTOR_HPP_INCLUDED

#include <vlite/common_vector_base.hpp>
#include <vlite/expr_vector.hpp>

#include <iterator>

namespace vlite
{

template <typename It, typename Op, std::size_t N>
class unary_expr_vector : public expr_vector<Op, N>,
                          public common_vector_base<unary_expr_vector<It, Op, N>>
{
  using iterator_result = decltype(*std::declval<It>());

public:
  using expr_vector<Op, N>::order;

  using value_type = std::decay_t<std::result_of_t<Op(iterator_result)>>;

  using typename expr_vector<Op, N>::size_type;

  using typename expr_vector<Op, N>::difference_type;

  class iterator
  {
    friend class unary_expr_vector<It, Op, N>;

  public:
    using iterator_category = std::input_iterator_tag;
    using value_type = std::decay_t<std::result_of_t<Op(iterator_result)>>;

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
      ++it_;
      return *this;
    }

    constexpr auto operator++(int) -> iterator
    {
      auto copy = *this;
      ++(*this);
      return copy;
    }

    constexpr auto operator==(const iterator& other) const { return it_ == other.it_; }

    constexpr auto operator!=(const iterator& other) const { return !(*this == other); }

    constexpr auto operator*() const -> value_type { return source_->operate(*it_); }

  private:
    constexpr iterator(It it, const unary_expr_vector* source)
      : it_{it}
      , source_{source}
    {
    }

    It it_;
    const unary_expr_vector* source_;
  };

  using const_iterator = iterator;

public:
  unary_expr_vector(It it_first, It it_last, Op op, std::size_t size)
    : expr_vector<Op, order>(std::move(op), size)
    , it_first_{it_first}
    , it_last_{it_last}
  {
  }

  unary_expr_vector(const unary_expr_vector& source) = delete;
  unary_expr_vector(unary_expr_vector&& source) noexcept = delete;

  auto operator=(const unary_expr_vector& source) -> unary_expr_vector& = delete;
  auto operator=(unary_expr_vector&& source) noexcept -> unary_expr_vector& = delete;

  auto begin() const -> const_iterator { return cbegin(); }
  auto end() const -> const_iterator { return cend(); }

  auto cbegin() const -> iterator { return {it_first_, this}; }
  auto cend() const -> iterator { return {it_last_, this}; }

  using expr_vector<Op, order>::size;
  using expr_vector<Op, order>::dimensions;
  using expr_vector<Op, order>::length;
  using expr_vector<Op, order>::row_count;
  using expr_vector<Op, order>::column_count;

  /// \exclude
  auto first() const { return it_first_; }

private:
  It it_first_, it_last_;
};

template <typename It, typename Op, std::size_t N>
unary_expr_vector(It, It, Op, std::size_t)->unary_expr_vector<It, Op, N>;

} // namespace vlite

#endif // VLITE_UNARY_EXPR_VECTOR_HPP_INCLUDED
