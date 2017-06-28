#ifndef VLITE_VECTOR_VECTOR_HPP_INCLUDED
#define VLITE_VECTOR_VECTOR_HPP_INCLUDED

#include <vlite/allocator.hpp>
#include <vlite/builder.hpp>
#include <vlite/functional.hpp>
#include <vlite/numeric.hpp>
#include <vlite/ref_vector.hpp>

namespace vlite
{

template <typename T> class vector : public ref_vector<T>, private allocator<T>
{
  template <typename> friend class vector;

public:
  using value_type = T;

  using iterator = value_type*;

  using const_iterator = const value_type*;

  using size_type = std::size_t;

  using difference_type = std::ptrdiff_t;

  explicit vector(std::size_t size)
    : ref_vector<value_type>{this->allocate(size)}
  {
    this->construct(this->block_);
  }

  vector(const value_type& value, std::size_t size)
    : ref_vector<value_type>{this->allocate(size)}
  {
    this->construct(this->block_, value);
  }

  vector(std::initializer_list<value_type> values)
    : ref_vector<value_type>{this->allocate(values.size())}
  {
    this->construct(this->block_, values.begin());
  }

  template <typename It, typename R = typename std::iterator_traits<It>::reference>
  vector(It begin, It end)
    : ref_vector<value_type>{this->allocate(std::distance(begin, end))}
  {
    static_assert(std::is_constructible<value_type, R>());
    this->construct(this->block_, begin, end);
  }

  explicit vector(uninitialized_t, std::size_t size = 0u)
    : ref_vector<value_type>{this->allocate(size)}
  {
    static_assert(std::is_pod_v<value_type>,
                  "Uninitialized vector is only allowed for POD types");
  }

  explicit vector(builder<value_type> b)
    : ref_vector<value_type>{b.release()}
  {
  }

  ~vector()
  {
    this->destroy(this->block_);
    this->deallocate(this->block_);
  }

  vector(const vector& source)
    : ref_vector<value_type>{this->allocate(source.size())}
  {
    this->construct(this->block_, source.data());
  }

  auto operator=(const vector& source) -> vector&
  {
    // no smart size check, it is dangerous.

    auto new_block = this->allocate(source.size());
    this->construct(new_block, source.data());
    this->destroy(this->block_);
    this->block_ = new_block;

    return *this;
  }

  vector(vector&& source) noexcept
    : ref_vector<value_type>{std::exchange(source.block_, {})}
  {
  }

  auto operator=(vector&& source) noexcept -> vector&
  {
    std::swap(this->block_, source.block_);
    return *this;
  }

  auto size() const noexcept -> std::size_t { return this->block_.size(); }

  auto data() noexcept -> value_type* { return this->block_.data(); }

  auto data() const noexcept -> const value_type* { return this->block_.data(); }
};

#if !defined(__clang__)
// clang-format off
template <class T> vector(const T&, std::size_t) -> vector<T>;
template <class T> vector(std::initializer_list<T>) -> vector<T>;
// clang-format on
#endif // __clang__

} // namespace vlite

#endif // VLITE_VECTOR_VECTOR_HPP_INCLUDED
