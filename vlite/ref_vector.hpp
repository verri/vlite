#ifndef VLITE_REF_VECTOR_HPP_INCLUDED
#define VLITE_REF_VECTOR_HPP_INCLUDED

#include <vlite/common_vector_base.hpp>
#include <vlite/memory_block.hpp>
#include <vlite/slice.hpp>
#include <vlite/strided_ref_vector.hpp>

#include <stdexcept>
#include <utility>

namespace vlite
{

template <typename T> class ref_vector : public common_vector_base<ref_vector<T>>
{
  template <typename> friend class ref_vector;

public:
  using value_type = T;

  using iterator = value_type*;

  using const_iterator = const value_type*;

  using size_type = std::size_t;

  using difference_type = std::ptrdiff_t;

  ref_vector(memory_block<value_type> block)
    : block_{std::move(block)}
  {
  }

  ~ref_vector() = default;

  template <typename Vector>
  auto operator=(const common_vector_base<Vector>& source) -> ref_vector&
  {
    static_assert(std::is_assignable_v<value_type&, const typename Vector::value_type&>,
                  "incompatible assignment");

    if (source.size() != block_.size())
      throw std::runtime_error{"sizes mismatch"};

    std::copy(source.begin(), source.end(), begin());
    return *this;
  }

  template <typename U, typename = meta::fallback<CommonVector<U>>>
  auto operator=(const U& source) -> ref_vector&
  {
    static_assert(std::is_assignable<value_type&, U>::value, "incompatible assignment");
    std::fill(begin(), end(), source);
    return *this;
  }

  auto operator=(const ref_vector& source) -> ref_vector&
  {
    if (source.size() != block_.size())
      throw std::runtime_error{"sizes mismatch"};

    std::copy(source.begin(), source.end(), this->begin());
    return *this;
  }

  operator ref_vector<const value_type>() const { return {{data(), size()}}; }

  operator strided_ref_vector<value_type>() { return {data(), size(), 1u}; }

  operator strided_ref_vector<const value_type>() const { return {data(), size(), 1u}; }

  auto operator[](size_type i) -> value_type&
  {
    assert(i < size());
    return data()[i];
  }

  auto operator[](size_type i) const -> const value_type&
  {
    assert(i < size());
    return data()[i];
  }

  auto operator[](every_index) -> ref_vector<value_type> { return *this; }

  auto operator[](every_index) const -> ref_vector<const value_type> { return *this; }

  auto operator[](slice s) -> ref_vector<value_type>
  {
    assert(s.start < size());
    assert(s.start + s.size <= size());
    return {{data() + s.start, s.size}};
  }

  auto operator[](slice s) const -> ref_vector<const value_type>
  {
    assert(s.start < size());
    assert(s.start + s.size <= size());
    return {{data() + s.start, s.size}};
  }

  auto operator[](strided_slice s) -> strided_ref_vector<value_type>
  {
    assert(s.start < size());
    assert(s.start + s.stride * s.size <= size());
    return {data() + s.start, s.size, s.stride};
  }

  auto operator[](strided_slice s) const -> strided_ref_vector<const value_type>
  {
    assert(s.start < size());
    assert(s.start + s.stride * s.size <= size());
    return {data() + s.start, s.size, s.stride};
  }

  auto operator[](bounded_slice s) -> ref_vector<value_type>
  {
    assert(s.start < size());
    return {{data() + s.start, s.size(size() - s.start)}};
  }

  auto operator[](bounded_slice s) const -> ref_vector<const value_type>
  {
    assert(s.start < size());
    return {{data() + s.start, s.size(size() - s.start)}};
  }

  auto operator[](strided_bounded_slice s) -> strided_ref_vector<value_type>
  {
    assert(s.start < size());

    auto maximum_size = size() - s.start;
    maximum_size += maximum_size % s.stride == 0u ? 0u : 1u;
    maximum_size /= s.stride;

    return {data() + s.start, s.size(maximum_size), s.stride};
  }

  auto operator[](strided_bounded_slice s) const -> strided_ref_vector<const value_type>
  {
    assert(s.start < size());

    auto maximum_size = size() - s.start;
    maximum_size += maximum_size % s.stride == 0u ? 0u : 1u;
    maximum_size /= s.stride;

    return {data() + s.start, s.size(maximum_size), s.stride};
  }

  auto begin() noexcept -> iterator { return data(); }
  auto end() noexcept -> iterator { return data() + size(); }

  auto begin() const noexcept -> const_iterator { return cbegin(); }
  auto end() const noexcept -> const_iterator { return cend(); }

  auto cbegin() const noexcept -> const_iterator { return data(); }
  auto cend() const noexcept -> const_iterator { return data() + size(); }

  auto size() const noexcept { return block_.size(); }

protected:
  auto data() -> value_type* { return block_.data(); }

  auto data() const -> const value_type* { return block_.data(); }

  ref_vector() = default;

  ref_vector(const ref_vector& source) = default;
  ref_vector(ref_vector&& source) noexcept = default;

  memory_block<value_type> block_;
};

} // namespace vlite

#endif // VLITE_REF_VECTOR_HPP_INCLUDED
