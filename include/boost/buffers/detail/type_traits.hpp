//
// Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/http_proto
//

#ifndef BOOST_BUFFERS_DETAIL_TYPE_TRAITS_HPP
#define BOOST_BUFFERS_DETAIL_TYPE_TRAITS_HPP

#include <boost/buffers/detail/config.hpp>
#include <iterator>
#include <type_traits>

namespace boost {
namespace buffers {
namespace detail {

template<class Base, class Derived>
using derived_from = std::integral_constant<bool,
    std::is_base_of<Base, Derived>::value &&
    std::is_convertible<
        Derived const volatile*,
        Base const volatile*>::value>;

//#if defined(__cpp_lib_concepts) && __cpp_lib_concepts >= 201907
//template<class T>
//using is_bidirectional_iterator = std::is_bidirectional_iterator<T>;
//#else

// C++11 version of is_bidirectional_iterator

// Alias for true_type if T is a BidirectionalIterator
template<class T, class = void>
struct is_bidirectional_iterator : std::false_type
{
};

template<class T>
struct is_bidirectional_iterator<T, detail::void_t<
    // ITER_TRAITS
    typename std::iterator_traits<T>::value_type,
    typename std::iterator_traits<T>::difference_type,
    typename std::iterator_traits<T>::reference,
    typename std::iterator_traits<T>::pointer,
    typename std::iterator_traits<T>::iterator_category,

    typename std::enable_if<

    // std::input_iterator
    std::is_same<decltype(*std::declval<T&>()), typename std::iterator_traits<T>::reference>::value &&

    // std::forward_iterator
    std::is_destructible<T>::value &&
    std::is_default_constructible<T>::value &&
    std::is_copy_constructible<T>::value &&
    std::is_copy_assignable<T>::value &&
    std::is_move_constructible<T>::value &&
    std::is_move_assignable<T>::value &&
    std::is_convertible<decltype(std::declval<T const&>()==std::declval<T const&>()), bool>::value &&
    std::is_convertible<decltype(std::declval<T const&>()!=std::declval<T const&>()), bool>::value &&
    std::is_signed<typename std::iterator_traits<T>::difference_type>::value && // stronger than needed
    std::is_same<decltype(++std::declval<T&>()), T&>::value &&
    std::is_same<decltype(std::declval<T&>()++), T>::value &&

    // std::bidirectional_iterator
    derived_from<std::bidirectional_iterator_tag, typename std::iterator_traits<T>::iterator_category>::value &&
    std::is_same<decltype(--std::declval<T&>()), T&>::value &&
    std::is_same<decltype(std::declval<T&>()--), T>::value

    >::type >>
    : std::true_type
{
};

//#endif

} // detail
} // buffers
} // boost

#endif
