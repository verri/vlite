#ifndef VLITE_REF_VECTOR_CONCEPT_HPP_INCLUDED
#define VLITE_REF_VECTOR_CONCEPT_HPP_INCLUDED

#include <vlite/common_vector_base.hpp>
#include <vlite/slice.hpp>

namespace vlite
{

namespace detail
{
template <typename T> using indexing = decltype(std::declval<T&>()[std::size_t{}]);
template <typename T> using all_indexing = decltype(std::declval<T&>()[all]);
template <typename T>
using slice_indexing = decltype(std::declval<T&>()[std::declval<slice>()]);
template <typename T>
using strided_indexing = decltype(std::declval<T&>()[std::declval<strided_slice>()]);
template <typename T>
using bounded_indexing = decltype(std::declval<T&>()[std::declval<bounded_slice>()]);
template <typename T>
using strided_bounded_indexing =
  decltype(std::declval<T&>()[std::declval<strided_bounded_slice>()]);
}

template <typename T, typename = void> struct RefVector : std::false_type
{
};

template <typename T>
struct CommonVector<                                       //
  T, meta::requires<                                       //
       CommonVector<T>,                                    //
       meta::compiles<T, detail::indexing>,                //
       meta::compiles<T, detail::all_indexing>,            //
       meta::compiles<T, detail::slice_indexing>,          //
       meta::compiles<T, detail::strided_indexing>,        //
       meta::compiles<T, detail::bounded_indexing>,        //
       meta::compiles<T, detail::strided_bounded_indexing> //
       >> : std::true_type
{
};

} // namespace vlite

#endif // VLITE_REF_VECTOR_CONCEPT_HPP_INCLUDED
