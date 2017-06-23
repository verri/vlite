#ifndef VLITE_ALLOCATOR_HPP_INCLUDED
#define VLITE_ALLOCATOR_HPP_INCLUDED

#include <vlite/memory_block.hpp>

#include <memory>

namespace vlite
{

struct uninitialized_t
{
};

static constexpr auto uninitialized = uninitialized_t{};

template <typename T> struct allocator
{
  using value_type = T;
  using storage_type = std::aligned_storage_t<sizeof(value_type), alignof(value_type)>;
  using block_type = memory_block<value_type>;

  auto allocate(std::size_t size) const -> block_type
  {
    return {reinterpret_cast<value_type*>(new storage_type[size]), size};
  }

  auto deallocate(block_type block) const noexcept -> void
  {
    delete[] reinterpret_cast<storage_type*>(block.data());
  }

  auto construct(block_type block) const
    noexcept(noexcept(new (std::declval<void*>()) value_type())) -> void
  {
    std::uninitialized_value_construct_n(block.data(), block.size());
  }

  auto construct(block_type block, const value_type& value) const
    noexcept(noexcept(new (std::declval<void*>()) value_type(value))) -> void
  {
    std::uninitialized_fill_n(block.data(), block.size(), value);
  }

  template <typename It, typename R = typename std::iterator_traits<It>::reference>
  auto construct(block_type block, It begin, It end) const
    noexcept(noexcept(new (std::declval<void*>()) value_type(*begin))) -> void
  {
    static_assert(std::is_constructible<value_type, R>());
    assert(std::distance(begin, end) == static_cast<std::ptrdiff_t>(block.size()));
    std::uninitialized_copy(begin, end, block.data());
  }

  template <typename It, typename R = typename std::iterator_traits<It>::reference>
  auto construct(block_type block, It begin) const
    noexcept(noexcept(new (std::declval<void*>()) value_type(*begin))) -> void
  {
    static_assert(std::is_constructible<value_type, R>());
    std::uninitialized_copy_n(begin, block.size(), block.data());
  }

  auto destroy(block_type block) const noexcept -> void
  {
    std::destroy_n(block.data(), block.size());
  }
};

} // namespace vlite

#ifdef DOCTEST_LIBRARY_INCLUDED

#include <algorithm>

TEST_CASE("[allocator] Allocation and deallocation")
{
  const auto allocator = vlite::allocator<double>{};
  const auto block = allocator.allocate(10);
  allocator.construct(block);

  const auto not_zero =
    std::find_if_not(block.begin(), block.end(), [](auto v) { return v == 0; });
  CHECK(not_zero == block.end());

  allocator.destroy(block);
  allocator.deallocate(block);
}

#endif // DOCTEST_LIBRARY_INCLUDED

#endif // VLITE_ALLOCATOR_HPP_INCLUDED
