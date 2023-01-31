//
// Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/http_proto
//

#ifndef BOOST_BUFFERS_DETAIL_TYPE_TRAITS_HPP
#define BOOST_BUFFERS_DETAIL_TYPE_TRAITS_HPP

#include <boost/buffers/detail/config.hpp>
#include <boost/type_traits/make_void.hpp>
#include <iterator>
#include <type_traits>

namespace boost {
namespace buffers {
namespace detail {

// is bidirectional iterator
template<class T, class = void>
struct is_bidirectional_iterator : std::false_type
{
};

template<class T>
struct is_bidirectional_iterator<T, boost::void_t<decltype(
    // LegacyIterator
    *std::declval<T&>()
    ),
    // LegacyIterator
    typename std::iterator_traits<T>::value_type,
    typename std::iterator_traits<T>::difference_type,
    typename std::iterator_traits<T>::reference,
    typename std::iterator_traits<T>::pointer,
    typename std::iterator_traits<T>::iterator_category,
    typename std::enable_if<
    // LegacyIterator
    std::is_copy_constructible<T>::value &&
    std::is_copy_assignable<T>::value &&
    std::is_destructible<T>::value &&
    std::is_same<T&, decltype(
        ++std::declval<T&>())>::value &&
    // Swappable
    //  VFALCO TODO
    // EqualityComparable
    std::is_convertible<decltype(
        std::declval<T const&>() ==
            std::declval<T const&>()),
        bool>::value &&
    // LegacyInputIterator
    std::is_convertible<typename
        std::iterator_traits<T>::reference, typename
        std::iterator_traits<T>::value_type>::value &&
    std::is_same<typename
        std::iterator_traits<T>::reference,
        decltype(*std::declval<T const&>())>::value &&
    std::is_convertible<decltype(
        std::declval<T const&>() !=
            std::declval<T const&>()),
        bool>::value &&
    std::is_same<T&, decltype(
        ++std::declval<T&>())>::value &&
    // VFALCO (void)r++   (void)++r
    std::is_convertible<decltype(
        *std::declval<T&>()++), typename
        std::iterator_traits<T>::value_type>::value &&
    // LegacyForwardIterator
    std::is_default_constructible<T>::value &&
    std::is_same<T, decltype(
        std::declval<T&>()++)>::value &&
    std::is_same<typename
        std::iterator_traits<T>::reference,
            decltype(*std::declval<T&>()++)
                >::value &&
    // LegacyBidirectionalIterator
    std::is_same<T&, decltype(
        --std::declval<T&>())>::value &&
    std::is_convertible<decltype(
        std::declval<T&>()--),
            T const&>::value &&
    std::is_same<typename
        std::iterator_traits<T>::reference,
        decltype(*std::declval<T&>()--)>::value
    >::type >>
    : std::true_type
{
};

} // detail
} // buffers
} // boost

#endif
