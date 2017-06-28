#ifndef VLITE_STRIDED_ITERATOR_HPP_INCLUDED
#define VLITE_STRIDED_ITERATOR_HPP_INCLUDED

#include <iterator>

namespace vlite
{
template <typename T> class strided_iterator
{
public:
  using iterator_category = std::random_access_iterator_tag;
  using value_type = T;
  using difference_type = std::ptrdiff_t;
  using pointer = T*;
  using reference = T&;

  constexpr strided_iterator() noexcept = default;

  constexpr strided_iterator(T* data, std::size_t stride) noexcept
    : data_{data}
    , stride_{stride}
  {
  }

  constexpr strided_iterator(const strided_iterator& source) noexcept = default;
  constexpr strided_iterator(strided_iterator&& source) noexcept = default;

  constexpr auto operator=(const strided_iterator& source) noexcept
    -> strided_iterator& = default;

  constexpr auto operator=(strided_iterator&& source) noexcept
    -> strided_iterator& = default;

  constexpr auto operator*() const noexcept -> reference { return data_[current_]; }
  constexpr auto operator-> () const noexcept -> pointer { return data_ + current_; }

  constexpr auto operator==(const strided_iterator& other) const noexcept -> bool
  {
    return data_ + current_ == other.data_ + other.current_;
  }

  constexpr auto operator!=(const strided_iterator& other) const noexcept -> bool
  {
    return !(*this == other);
  }

  constexpr auto operator++() noexcept -> strided_iterator&
  {
    current_ += stride_;
    return *this;
  }

  constexpr auto operator++(int)noexcept -> strided_iterator
  {
    auto copy = *this;
    ++(*this);
    return copy;
  }

  constexpr auto operator--() noexcept -> strided_iterator&
  {
    current_ -= stride_;
    return *this;
  }

  constexpr auto operator--(int)noexcept -> strided_iterator
  {
    auto copy = *this;
    --(*this);
    return copy;
  }

  constexpr auto operator+(difference_type n) const noexcept -> strided_iterator
  {
    auto copy = *this;
    copy.current_ += n * copy.stride_;
    return copy;
  }

  constexpr auto operator-(difference_type n) const noexcept -> strided_iterator
  {
    auto copy = *this;
    copy.current_ -= n * copy.stride_;
    return copy;
  }

private:
  T* data_ = nullptr;
  std::size_t stride_ = 0u;
  std::size_t current_ = 0u;
};

template <typename T>
constexpr auto operator+(typename strided_iterator<T>::difference_type n,
                         const strided_iterator<T>& iterator) noexcept
  -> strided_iterator<T>
{
  return iterator + n;
}

template <typename T>
constexpr auto operator-(typename strided_iterator<T>::difference_type n,
                         const strided_iterator<T>& iterator) noexcept
  -> strided_iterator<T>
{
  return iterator - n;
}

} // namespace vlite

#endif // VLITE_STRIDED_ITERATOR_HPP_INCLUDED
