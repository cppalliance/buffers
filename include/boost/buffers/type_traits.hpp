//
// Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/buffers
//

#ifndef BOOST_BUFFERS_TYPE_TRAITS_HPP
#define BOOST_BUFFERS_TYPE_TRAITS_HPP

#include <boost/buffers/detail/config.hpp>
#include <boost/buffers/buffer.hpp>

namespace boost {
namespace buffers {

/** Determine if T is a DynamicBuffer
*/
template<
    class T,
    class = void>
struct is_dynamic_buffer : std::false_type {};

template<class T>
struct is_dynamic_buffer<
    T, void_t<decltype(
        std::declval<std::size_t&>() =
            std::declval<T const&>().size()
        ,std::declval<std::size_t&>() =
            std::declval<T const&>().max_size()
        ,std::declval<std::size_t&>() =
            std::declval<T const&>().capacity()
        ,std::declval<T&>().commit(
            std::declval<std::size_t>())
        ,std::declval<T&>().consume(
            std::declval<std::size_t>())
    )
    ,typename std::enable_if<
        is_const_buffer_sequence<typename
            T::const_buffers_type>::value
        && is_mutable_buffer_sequence<typename
            T::mutable_buffers_type>::value
        >::type
    ,typename std::enable_if<
        std::is_same<decltype(
            std::declval<T const&>().data()),
            typename T::const_buffers_type>::value
        && std::is_same<decltype(
            std::declval<T&>().prepare(
                std::declval<std::size_t>())),
            typename T::mutable_buffers_type>::value
        >::type
    > > : std::true_type
{
};

/** Return the underlying buffer type of a sequence.
*/
template<class T>
using value_type = typename
    std::conditional<
        is_mutable_buffer_sequence<T>::value,
        mutable_buffer,
        const_buffer
            >::type;

} // buffers
} // boost

#endif
