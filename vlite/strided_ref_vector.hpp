#ifndef VLITE_STRIDED_REF_VECTOR_HPP_INCLUDED
#define VLITE_STRIDED_REF_VECTOR_HPP_INCLUDED

#include <vlite/common_vector_base.hpp>
#include <vlite/strided_iterator.hpp>

namespace vlite
{

template <typename T>
class strided_ref_vector : public common_vector_base<strided_ref_vector<T>>
{
  template <typename, typename> friend class strided_ref_vector;

public:
  using value_type = T;

  using iterator = strided_iterator<T>;

  using const_iterator = strided_iterator<const T>;

  using size_type = std::size_t;

  using difference_type = std::ptrdiff_t;

  strided_ref_vector(value_type* data, std::size_t size, std::size_t stride)
    : data_{data}
    , size_{size}
    , stride_{stride}
  {
  }

  ~strided_ref_vector() {} // not default to disable default copy, move, assignment, ...

  template <typename Vector>
  auto operator=(const common_vector_base<Vector>& source) -> strided_ref_vector&
  {
    static_assert(std::is_assignable_v<value_type&, const typename Vector::value_type&>,
                  "incompatible assignment");

    if (source.size() != this->size())
      throw std::runtime_error{"sizes mismatch"};

    std::copy(source.begin(), source.end(), begin());

    return *this;
  }

  template <typename U, typename = meta::fallback<CommonVector<U>>>
  auto operator=(const U& source) -> strided_ref_vector&
  {
    static_assert(std::is_assignable<value_type&, U>::value, "incompatible assignment");
    std::fill(begin(), end(), source);
    for (auto& elem : *this)
      elem = source;
    return *this;
  }

  auto operator=(const strided_ref_vector& source) -> strided_ref_vector&
  {
    if (source.size() != size())
      throw std::runtime_error{"sizes mismatch"};

    std::copy(source.begin(), source.end(), this->begin());
    return *this;
  }

  operator strided_ref_vector<const value_type>() const
  {
    return {data(), size(), stride()};
  }

  auto operator[](size_type i) -> value_type& { return data()[i * stride()]; }

  auto operator[](size_type i) const -> const value_type& { return data()[i * stride()]; }

  // decltype(auto) operator[](absolute_slice slice)
  // {
  //   return detail::vector_at(data(), mapper(), std::move(slice));
  // }

  // decltype(auto) operator[](absolute_slice slice) const
  // {
  //   return detail::vector_at(data(), mapper(), std::move(slice));
  // }

  // decltype(auto) operator[](absolute_strided_slice slice)
  // {
  //   return detail::vector_at(data(), mapper(), std::move(slice));
  // }

  // decltype(auto) operator[](absolute_strided_slice slice) const
  // {
  //   return detail::vector_at(data(), mapper(), std::move(slice));
  // }

  // decltype(auto) operator[](every_index index)
  // {
  //   return detail::vector_at(data(), mapper(), index);
  // }

  // decltype(auto) operator[](every_index index) const
  // {
  //   return detail::vector_at(data(), mapper(), index);
  // }

  // decltype(auto) operator[](bounded_slice slice)
  // {
  //   return detail::vector_at(data(), mapper(),
  //                           eval(slice, this->descriptor().extents[0u]));
  // }

  // decltype(auto) operator[](bounded_slice slice) const
  // {
  //   return detail::vector_at(data(), mapper(),
  //                           eval(slice, this->descriptor().extents[0u]));
  // }

  // decltype(auto) operator[](bounded_strided_slice slice)
  // {
  //   return detail::vector_at(data(), mapper(),
  //                           eval(slice, this->descriptor().extents[0u]));
  // }

  // decltype(auto) operator[](bounded_strided_slice slice) const
  // {
  //   return detail::vector_at(data(), mapper(),
  //                           eval(slice, this->descriptor().extents[0u]));
  // }

  // template <typename Rng, typename = meta::requires<range::SizedRange<Rng>>>
  // decltype(auto) operator[](const Rng& rng)
  // {
  //   return detail::vector_at(data(), mapper(), rng);
  // }

  // template <typename Rng, typename = meta::requires<range::SizedRange<Rng>>>
  // decltype(auto) operator[](const Rng& rng) const
  // {
  //   return detail::vector_at(data(), mapper(), rng);
  // }

  auto begin() noexcept -> iterator { return {data(), stride()}; }
  auto end() noexcept -> iterator { return {data() + stride() * size(), stride()}; }

  auto begin() const noexcept -> const_iterator { return cbegin(); }
  auto end() const noexcept -> const_iterator { return cend(); }

  auto cbegin() const noexcept -> const_iterator { return {data(), stride()}; }
  auto cend() const noexcept -> const_iterator
  {
    return {data() + stride() * size(), stride()};
  }

  auto size() const noexcept { return size_; }

protected:
  auto data() -> value_type* { return data_; }

  auto data() const -> const value_type* { return data_; }

  auto stride() const -> std::size_t { return stride_; }

  strided_ref_vector(const strided_ref_vector& source) = default;
  strided_ref_vector(strided_ref_vector&& source) noexcept = default;

  value_type* data_;
  std::size_t size_;
  std::size_t stride_;
};

} // namespace vlite

#endif // VLITE_STRIDED_REF_VECTOR_HPP_INCLUDED
