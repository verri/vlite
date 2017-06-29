#ifndef VLITE_VECTOR_VECTOR_HPP_INCLUDED
#define VLITE_VECTOR_VECTOR_HPP_INCLUDED

#include <vlite/allocator.hpp>
#include <vlite/builder.hpp>
#include <vlite/functional.hpp>
#include <vlite/numeric.hpp>
#include <vlite/ref_vector.hpp>
#include <vlite/ref_vector_concept.hpp>

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

  template <typename Vector>
  vector(const common_vector_base<Vector>& other)
    : ref_vector<value_type>{this->allocate(other.size())}
  {
    static_assert(std::is_constructible_v<T, const typename Vector::value_type&>);
    this->construct(this->block_, other.begin());
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

  auto data() noexcept -> value_type* { return this->block_.data(); }

  auto data() const noexcept -> const value_type* { return this->block_.data(); }

  using ref_vector<T>::size;
  using ref_vector<T>::begin;
  using ref_vector<T>::end;
  using ref_vector<T>::cbegin;
  using ref_vector<T>::cend;
};

template <typename T> vector(const T&, std::size_t)->vector<T>;
template <typename T> vector(std::initializer_list<T>)->vector<T>;
template <typename Vector>
vector(common_vector_base<Vector>)->vector<std::decay_t<typename Vector::value_type>>;

template <typename Vector, typename = meta::requires<RefVector<Vector>>>
constexpr auto ref(Vector vec)
{
  return vec;
}

template <typename T> constexpr auto ref(vector<T>& vec) { return vec[every]; }

template <typename T> constexpr auto ref(const vector<T>& vec) { return vec[every]; }

} // namespace vlite

#ifdef DOCTEST_LIBRARY_INCLUDED

#include <algorithm>
#include <complex>

using test_types =
  doctest::Types<char, short, int, long, double, float, std::complex<float>>;

TEST_CASE_TEMPLATE("[vector] Value-initialized vector", T, test_types)
{
  auto vector = vlite::vector<T>(10u);

  CHECK(vector.size() == 10u);
  CHECK(std::find_if_not(vector.begin(), vector.end(),
                         [](const auto& x) { return x == T{}; }) == vector.end());
}

template <typename Vector>
auto is_ref_vector(Vector) -> vlite::meta::requires_t<bool, vlite::RefVector<Vector>>
{
  return true;
}

template <typename U>
auto is_ref_vector(U) -> vlite::meta::fallback_t<bool, vlite::RefVector<U>>
{
  return false;
}

TEST_CASE("[vector] Check if is a reference to vector")
{
  using namespace vlite;

  const auto a = vector{1, 2, 3};
  auto b = vector{1.0, 2.0, 3.0};

  CHECK(a.size() == 3u);
  CHECK(b.size() == 3u);

  CHECK_FALSE(is_ref_vector(a));
  CHECK_FALSE(is_ref_vector(b));

  static_assert(CommonVector<vector<int>>::value);
  static_assert(CommonVector<ref_vector<int>>::value);

  CHECK(is_ref_vector(ref(a)));
  CHECK(is_ref_vector(a[every]));
  CHECK(is_ref_vector(a[{0, 2}]));
  CHECK(is_ref_vector(a[{0, 2, 1}]));
  CHECK(is_ref_vector(a[{0, at_most(100u), 1}]));
  CHECK(is_ref_vector(a[{0, every}]));
  CHECK(is_ref_vector(a[{0, every, 1u}]));

  CHECK(is_ref_vector(ref(b)));
  CHECK(is_ref_vector(b[every]));
  CHECK(is_ref_vector(b[{0, 2}]));
  CHECK(is_ref_vector(b[{0, 2, 1}]));
  CHECK(is_ref_vector(b[{0, at_most(100u), 1}]));
  CHECK(is_ref_vector(b[{0, every}]));
  CHECK(is_ref_vector(b[{0, every, 1u}]));
}

TEST_CASE("[vector] Assignment")
{
  using namespace vlite;

  auto a = vector<int>(10u);
  a[every] = 1;

  CHECK(a.size() == 10u);
  CHECK(std::find_if_not(a.begin(), a.end(), [](const auto& x) { return x == 1; }) ==
        a.end());
}

template <typename Vector, typename = vlite::meta::requires<vlite::RefVector<Vector>>>
auto fill_with_default(Vector vec)
{
  vec = typename Vector::value_type{};
}

TEST_CASE("[vector] Fill")
{
  auto a = vlite::vector{1, 2, 3};
  fill_with_default(ref(a));

  CHECK(a.size() == 3u);
  CHECK(std::find_if_not(a.begin(), a.end(), [](const auto& x) { return x == 0; }) ==
        a.end());
}

TEST_CASE("[vector] Operations")
{
  using namespace vlite;

  auto a = vector<int>(3u);

  CHECK(all(a == 0));
  CHECK(all(a + 1 == 1));

  a[0] = 1;
  a[{1, every}] = 2;

  CHECK(all(-a == vector{-1, -2, -2}));
  CHECK(none(0 == a));
  CHECK(any(a == 1));
}

#endif // DOCTEST_LIBRARY_INCLUDED

#endif // VLITE_VECTOR_VECTOR_HPP_INCLUDED
