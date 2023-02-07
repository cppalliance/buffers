//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/http_proto
//

#ifndef BOOST_BUFFERS_IMPL_SOURCE_HPP
#define BOOST_BUFFERS_IMPL_SOURCE_HPP

#include <boost/buffers/iterators.hpp>
#include <boost/buffers/type_traits.hpp>
#include <boost/buffers/detail/except.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace buffers {

inline
auto
source::
results::
operator+=(
    results const& rv) ->
        results&
{
    BOOST_ASSERT(! rv.ec.failed());
    BOOST_ASSERT(! rv.finished);
    ec = rv.ec;
    bytes += rv.bytes;
    finished = rv.finished;
    return *this;
}

//------------------------------------------------

inline
auto
source::
read(
    mutable_buffer b) ->
        results
{
    // Forgot to call init
    if(! is_inited())
        detail::throw_logic_error();

    // Already finished
    if(is_finished())
        detail::throw_logic_error();

    auto rv = do_read_one(
        b.data(), b.size());
    finished_ = rv.finished;
    return rv;
}

inline
auto
source::
read(
    mutable_buffer const* dest,
    std::size_t dest_len) ->
        results
{
    // Forgot to call init
    if(! is_inited())
        detail::throw_logic_error();

    // Already finished
    if(is_finished())
        detail::throw_logic_error();

    auto rv = do_read(dest, dest_len);
    finished_ = rv.finished;
    return rv;
}

template<class MutableBufferSequence>
auto
source::
read(MutableBufferSequence const& dest) ->
    results
{
    static_assert(
        is_mutable_buffer_sequence<
            MutableBufferSequence>::value,
        "Type requirements not met");

    results rv;
    constexpr int SmallArraySize = 16;
    mutable_buffer tmp[SmallArraySize];
    auto const tmp_end =
        tmp + SmallArraySize;
    auto it = (begin)(dest);
    auto const end_ = (end)(dest);
    while(it != end_)
    {
        auto p = tmp;
        do
        {
            *p++ = *it++;
        }
        while(
            p != tmp_end &&
            it != end_);
        auto saved = rv.bytes;
        rv = read(tmp, p - tmp);
        rv.bytes += saved;
        if(rv.ec.failed())
            return rv;
        if(rv.finished)
            break;
    }
    return rv;
}

} // buffers
} // boost

#endif
