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

namespace boost {
namespace buffers {

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
