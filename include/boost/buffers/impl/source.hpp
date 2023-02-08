//
// Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/buffers
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
    BOOST_ASSERT(! ec.failed());
    BOOST_ASSERT(! finished);
    ec = rv.ec;
    bytes += rv.bytes;
    finished = rv.finished;
    return *this;
}

//------------------------------------------------

template<class MutableBufferSequence>
auto
source::
read(
    MutableBufferSequence const& bs) ->
        results
{
    static_assert(
        is_mutable_buffer_sequence<
            MutableBufferSequence>::value,
        "Type requirements not met");

    // Forgot to call init
    if(! is_inited())
        detail::throw_logic_error();

    // Already finished
    if(is_finished())
        detail::throw_logic_error();

    auto rv = read_impl(bs);
    finished_ = rv.finished;
    return rv;
}

inline
auto
source::
read_impl(
    mutable_buffer const& b) ->
        results
{
    return on_read(
        mutable_buffer_span(&b, 1));
}

template<class T>
auto
source::
read_impl(
    T const& bs) ->
        results
{
    results rv;
    constexpr int SmallArraySize = 16;
    mutable_buffer tmp[SmallArraySize];
    auto const tmp_end =
        tmp + SmallArraySize;
    auto it = (begin)(bs);
    auto const end_ = (end)(bs);
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
        rv += read_impl(
            mutable_buffer_span(
                tmp, p - tmp));
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
