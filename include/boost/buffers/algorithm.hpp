//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/http_proto
//

#ifndef BOOST_BUFFERS_ALGORITHM_HPP
#define BOOST_BUFFERS_ALGORITHM_HPP

#include <boost/buffers/detail/config.hpp>
#include <boost/buffers/buffer_size.hpp>
#include <boost/buffers/const_buffer.hpp>
#include <boost/buffers/mutable_buffer.hpp>
#include <boost/buffers/tag_invoke.hpp>
#include <boost/buffers/type_traits.hpp>
#include <memory>

namespace boost {
namespace buffers {

#ifndef BOOST_BUFFERS_DOCS
template<class BufferSequence>
void
tag_invoke(
    prefix_tag const&,
    BufferSequence const&,
    std::size_t) = delete;
#endif

/** Returns the type of a prefix of a buffer sequence
*/
#ifdef BOOST_BUFFERS_DOCS
template<class BufferSequence>
using prefix_type = __see_below__;
#else
template<class BufferSequence>
using prefix_type = decltype(
    tag_invoke(
        prefix_tag{},
        std::declval<BufferSequence const&>(),
        std::size_t{}));
#endif

/** Return a prefix of the buffers.
*/
template<class BufferSequence>
auto
prefix(
    BufferSequence const& b,
    std::size_t n) ->
        prefix_type<BufferSequence>
{
    static_assert(
        is_const_buffer_sequence<BufferSequence>::value,
        "Type requirements not met");

    return tag_invoke(
        prefix_tag{}, b, n);
}

/** Return a prefix of the buffers.
*/
template<class BufferSequence>
auto
sans_suffix(
    BufferSequence const& b,
    std::size_t n) ->
        prefix_type<BufferSequence>
{
    auto const n0 = buffer_size(b);
    if( n > n0)
        n = n0;
    return prefix(b, n0 - n);
}

#ifndef BOOST_BUFFERS_DOCS
template<class BufferSequence>
void
tag_invoke(
    suffix_tag const&,
    BufferSequence const&,
    std::size_t) = delete;
#endif

/** Returns the type of a suffix of BufferSequence.
*/
#ifdef BOOST_BUFFERS_DOCS
template<class BufferSequence>
using suffix_type = __see_below__;
#else
template<class BufferSequence>
using suffix_type = decltype(
    tag_invoke(
        suffix_tag{},
        std::declval<BufferSequence const&>(),
        std::size_t{}));
#endif

/** Return a suffix of the buffers.
*/
template<class BufferSequence>
auto
suffix(
    BufferSequence const& b,
    std::size_t n) ->
        suffix_type<BufferSequence>   
{
    static_assert(
        is_const_buffer_sequence<BufferSequence>::value,
        "Type requirements not met");

    return tag_invoke(
        suffix_tag{}, b, n);
}

/** Return a suffix of the buffers.
*/
template<class BufferSequence>
auto
sans_prefix(
    BufferSequence const& b,
    std::size_t n) ->
        suffix_type<BufferSequence>
{
    static_assert(
        is_const_buffer_sequence<BufferSequence>::value,
        "Type requirements not met");

    auto const n0 = buffer_size(b);
    if( n > n0)
        n = n0;
    return suffix(b, n0 - n);
}

} // buffers
} // boost

#endif
