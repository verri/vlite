#ifndef VLITE_SLICE_HPP_INCLUDED
#define VLITE_SLICE_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <utility>

namespace vlite
{

struct strided_slice
{
  strided_slice() = delete;

  constexpr strided_slice(std::size_t start, std::size_t size,
                          std::size_t stride) noexcept
    : start{start}
    , size{size}
    , stride{stride}
  {
  }

  std::size_t start;
  std::size_t size;
  std::size_t stride;
};

struct slice
{
  slice() = delete;

  constexpr slice(std::size_t start, std::size_t size) noexcept
    : start{start}
    , size{size}
  {
  }

  constexpr operator strided_slice() const noexcept { return {start, size, 1u}; }

  std::size_t start;
  std::size_t size;
};

struct all_index
{
};

static constexpr auto all = all_index{};

class bounded_index
{
public:
  friend constexpr auto at_most(std::size_t maximum) noexcept -> bounded_index;

  constexpr auto operator()(std::size_t size) const noexcept
  {
    return std::min(maximum_, size);
  }

  constexpr bounded_index(all_index) noexcept
    : maximum_{std::numeric_limits<std::size_t>::max()}
  {
  }

  constexpr bounded_index(const bounded_index&) noexcept = default;
  constexpr bounded_index(bounded_index&&) noexcept = default;

  constexpr auto operator=(const bounded_index&) noexcept -> bounded_index& = default;
  constexpr auto operator=(bounded_index&&) noexcept -> bounded_index& = default;

private:
  constexpr bounded_index(int, std::size_t maximum) noexcept
    : maximum_{maximum}
  {
  }
  std::size_t maximum_;
};

constexpr auto at_most(std::size_t maximum) noexcept -> bounded_index
{
  return {{}, maximum};
}

struct strided_bounded_slice
{

  strided_bounded_slice() = delete;

  constexpr strided_bounded_slice(std::size_t start, bounded_index size,
                                  std::size_t stride) noexcept
    : start{start}
    , size{std::move(size)}
    , stride{stride}
  {
  }

  std::size_t start;
  bounded_index size;
  std::size_t stride;
};

struct bounded_slice
{

  bounded_slice() = delete;

  constexpr bounded_slice(std::size_t start, bounded_index size) noexcept
    : start{start}
    , size{std::move(size)}
  {
  }

  constexpr operator strided_bounded_slice() const noexcept { return {start, size, 1u}; }

  std::size_t start;
  bounded_index size;
};

} // namespace vlite

#endif // VLITE_SLICE_HPP_INCLUDED
