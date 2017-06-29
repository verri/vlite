#ifndef VLITE_BUILDER_HPP_INCLUDED
#define VLITE_BUILDER_HPP_INCLUDED

#include <vlite/allocator.hpp>

#include <cassert>

namespace vlite
{

template <typename T> class builder : private allocator<T>
{
public:
  using value_type = T;

  explicit builder(std::size_t size)
    : block_{this->allocate(size)}
  {
  }

  ~builder()
  {
    if (block_.data())
    {
      this->destroy({block_.data(), count_});
      this->deallocate(block_);
    }
  }

  builder(const builder&) = delete;

  builder(builder&& source) noexcept
    : block_{std::exchange(source.block_, {})}
    , count_{std::exchange(source.count_, 0u)}
  {
  }

  auto operator=(const builder&) -> builder& = delete;

  auto operator=(builder&& source) noexcept -> builder&
  {
    std::swap(block_, source.block_);
    std::swap(count_, source.count_);
  }

  auto is_complete() const noexcept { return count_ == block_.size(); }

  auto push_back(const T& value)
  {
    if (is_complete())
      throw std::runtime_error{"builder is already complete"};

    ::new (static_cast<void*>(block_.data() + count_)) T(value);
    ++count_;
  }

  auto push_back(T&& value)
  {
    if (is_complete())
      throw std::runtime_error{"builder is already complete"};

    ::new (static_cast<void*>(block_.data() + count_)) T(std::move(value));
    ++count_;
  }

  template <typename... Args> auto emplace_back(Args&&... args)
  {
    if (is_complete())
      throw std::runtime_error{"builder is already complete"};

    ::new (static_cast<void*>(block_.data() + count_)) T(std::forward<Args>(args)...);
    ++count_;
  }

  auto size() const noexcept { return block_.size(); }

  auto count() const noexcept { return count_; }

  auto release()
  {
    if (!is_complete())
      throw std::runtime_error{"builder is not complete"};

    count_ = 0u;
    return std::exchange(block_, {});
  }

private:
  memory_block<value_type> block_;
  std::size_t count_ = 0u;
};

} // namespace vlite

#ifdef DOCTEST_LIBRARY_INCLUDED

TEST_CASE("[builder] Building an vector in parts")
{
  auto a = vlite::builder<double>(10u);
  CHECK_THROWS(a.release());

  auto b = vlite::builder<double>(0u);
  CHECK(b.is_complete());

  auto c = vlite::builder<double>(20u);
  while (!c.is_complete())
    c.push_back(0.0);

  const auto block = c.release();

  const auto allocator = vlite::allocator<double>{};
  allocator.destroy(block);
  allocator.deallocate(block);
}

#endif // DOCTEST_LIBRARY_INCLUDED

#endif // VLITE_BUILDER_HPP_INCLUDED
