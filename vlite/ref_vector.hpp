#ifndef VLITE_REF_VECTOR_HPP_INCLUDED
#define VLITE_REF_VECTOR_HPP_INCLUDED

#include <vlite/common_vector_base.hpp>
#include <vlite/memory_block.hpp>
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

  ~ref_vector(){};

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
    for (auto& elem : *this)
      elem = source;
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

  auto operator[](size_type i) -> value_type& { return data()[i]; }

  auto operator[](size_type i) const -> const value_type& { return data()[i]; }

  // decltype(auto) operator[](absolute_slice slice)
  // {
  //   return detail::vector_at(data(), descriptor_, std::move(slice));
  // }

  // decltype(auto) operator[](absolute_slice slice) const
  // {
  //   return detail::vector_at(data(), descriptor_, std::move(slice));
  // }

  // decltype(auto) operator[](absolute_strided_slice slice)
  // {
  //   return detail::vector_at(data(), descriptor_, std::move(slice));
  // }

  // decltype(auto) operator[](absolute_strided_slice slice) const
  // {
  //   return detail::vector_at(data(), descriptor_, std::move(slice));
  // }

  // decltype(auto) operator[](every_index index)
  // {
  //   return detail::vector_at(data(), descriptor_, index);
  // }

  // decltype(auto) operator[](every_index index) const
  // {
  //   return detail::vector_at(data(), descriptor_, index);
  // }

  // decltype(auto) operator[](bounded_slice slice)
  // {
  //   return detail::vector_at(data(), descriptor_, eval(slice,
  //   descriptor_.extents[0u]));
  // }

  // decltype(auto) operator[](bounded_slice slice) const
  // {
  //   return detail::vector_at(data(), descriptor_, eval(slice,
  //   descriptor_.extents[0u]));
  // }

  // decltype(auto) operator[](bounded_strided_slice slice)
  // {
  //   return detail::vector_at(data(), descriptor_, eval(slice,
  //   descriptor_.extents[0u]));
  // }

  // decltype(auto) operator[](bounded_strided_slice slice) const
  // {
  //   return detail::vector_at(data(), descriptor_, eval(slice,
  //   descriptor_.extents[0u]));
  // }

  // template <typename Rng, typename = meta::requires<range::SizedRange<Rng>>>
  // decltype(auto) operator[](const Rng& rng)
  // {
  //   return detail::vector_at(data(), descriptor_, rng);
  // }

  // template <typename Rng, typename = meta::requires<range::SizedRange<Rng>>>
  // decltype(auto) operator[](const Rng& rng) const
  // {
  //   return detail::vector_at(data(), descriptor_, rng);
  // }

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
