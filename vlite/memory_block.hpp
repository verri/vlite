#ifndef VLITE_MEMORY_BLOCK_HPP_INCLUDED
#define VLITE_MEMORY_BLOCK_HPP_INCLUDED

#include <cstddef>

namespace vlite
{

template <typename T> class memory_block
{
public:
  using value_type = T;

  constexpr memory_block() = default;

  constexpr memory_block(value_type* data, std::size_t size)
    : data_{data}
    , size_{size}
  {
  }

  constexpr auto data() const noexcept -> value_type* { return data_; }
  constexpr auto size() const noexcept -> std::size_t { return size_; }

  constexpr auto begin() noexcept { return data_; }
  constexpr auto end() noexcept { return data_ + size_; }

  constexpr auto begin() const noexcept { return data_; }
  constexpr auto end() const noexcept { return data_ + size_; }

  constexpr auto cbegin() const noexcept { return data_; }
  constexpr auto cend() const noexcept { return data_ + size_; }

private:
  T* data_ = nullptr;
  std::size_t size_ = 0u;
};
} // namespace vlite

#endif // VLITE_MEMORY_BLOCK_HPP_INCLUDED
