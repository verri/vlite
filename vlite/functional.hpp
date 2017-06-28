#ifndef JULES_ARRAY_FUNCTIONAL_H
#define JULES_ARRAY_FUNCTIONAL_H

#include <vlite/binary_expr_vector.hpp>
#include <vlite/unary_expr_vector.hpp>

#include <functional>

namespace vlite
{

template <typename Vector, typename Op>
static auto apply(const common_vector_base<Vector>& operand, Op op)
{
  return unary_expr_vector(operand.begin(), operand.end(), std::move(op),
                           operand.dimensions());
}

template <typename VectorA, typename VectorB, typename Op>
static auto apply(const common_vector_base<VectorA>& lhs,
                  const common_vector_base<VectorB>& rhs, Op op)
{
  return binary_expr_vector(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), std::move(op),
                            lhs.dimensions());
}

#define OPERATIONS_LIST                                                                  \
  UNARY_OPERATIONS_LIST                                                                  \
  BINARY_OPERATIONS_LIST

#define BINARY_OPERATIONS_LIST                                                           \
  BINARY_COMBINATIONS(+, std::plus<>)                                                    \
  BINARY_COMBINATIONS(-, std::minus<>)                                                   \
  BINARY_COMBINATIONS(*, std::multiplies<>)                                              \
  BINARY_COMBINATIONS(/, std::divides<>)                                                 \
  BINARY_COMBINATIONS(%, std::modulus<>)                                                 \
  BINARY_COMBINATIONS(&&, std::logical_and<>)                                            \
  BINARY_COMBINATIONS(||, std::logical_or<>)                                             \
  BINARY_COMBINATIONS(==, std::equal_to<>)                                               \
  BINARY_COMBINATIONS(!=, std::not_equal_to<>)                                           \
  BINARY_COMBINATIONS(<, std::less<>)                                                    \
  BINARY_COMBINATIONS(<=, std::less_equal<>)                                             \
  BINARY_COMBINATIONS(>, std::greater<>)                                                 \
  BINARY_COMBINATIONS(>=, std::greater_equal<>)

#define BINARY_COMBINATIONS(OP__, FUNCTOR__)                                             \
  BINARY_OPERATION(OP__, FUNCTOR__)                                                      \
  BINARY_RIGHT_TYPE_OPERATION(OP__, FUNCTOR__)                                           \
  BINARY_LEFT_TYPE_OPERATION(OP__, FUNCTOR__)

#define BINARY_OPERATION(OP__, FUNCTOR__)                                                \
  template <typename VectorA, typename VectorB>                                          \
  auto operator OP__(const common_vector_base<VectorA>& lhs,                             \
                     const common_vector_base<VectorB>& rhs)                             \
  {                                                                                      \
    return apply(lhs, rhs, FUNCTOR__{});                                                 \
  }

#define BINARY_RIGHT_TYPE_OPERATION(OP__, FUNCTOR__)                                     \
  template <typename Vector, typename T, typename = meta::fallback<CommonVector<T>>>     \
  auto operator OP__(const common_vector_base<Vector>& lhs, T rhs)                       \
  {                                                                                      \
    return apply(lhs, [ rhs = std::move(rhs), op = FUNCTOR__{} ](auto&& value) {         \
      return op(std::forward<decltype(value)>(value), rhs);                              \
    });                                                                                  \
  }

#define BINARY_LEFT_TYPE_OPERATION(OP__, FUNCTOR__)                                      \
  template <typename T, typename Vector, typename = meta::fallback<CommonVector<T>>>     \
  auto operator OP__(T lhs, const common_vector_base<Vector>& rhs)                       \
  {                                                                                      \
    return apply(rhs, [ lhs = std::move(lhs), op = FUNCTOR__{} ](auto&& value) {         \
      return op(lhs, std::forward<decltype(value)>(value));                              \
    });                                                                                  \
  }

#define UNARY_OPERATIONS_LIST                                                            \
  UNARY_OPERATION(-, std::negate<>)                                                      \
  UNARY_OPERATION(!, std::logical_not<>)

#define UNARY_OPERATION(OP__, FUNCTOR__)                                                 \
  template <typename Vector>                                                             \
  auto operator OP__(const common_vector_base<Vector>& operand)                          \
  {                                                                                      \
    return apply(operand, FUNCTOR__{});                                                  \
  }

OPERATIONS_LIST

#undef UNARY_OPERATION
#undef UNARY_OPERATIONS_LIST

#undef BINARY_OPERATION
#undef BINARY_LEFT_TYPE_OPERATION
#undef BINARY_RIGHT_TYPE_OPERATION
#undef BINARY_COMBINATIONS
#undef BINARY_OPERATIONS_LIST

#undef OPERATIONS_LIST

} // namespace vlite

#endif // JULES_ARRAY_FUNCTIONAL_H
