//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/http_proto
//

#ifndef BOOST_BUFFERS_IMPL_SINK_HPP
#define BOOST_BUFFERS_IMPL_SINK_HPP

#include <boost/buffers/mutable_buffer.hpp>
#include <boost/buffers/iterators.hpp>
#include <boost/buffers/type_traits.hpp>
#include <boost/buffers/detail/except.hpp>

namespace boost {
namespace buffers {

template<class ConstBufferSequence>
auto
sink::
write(
    ConstBufferSequence const& src,
    bool more) ->
    results
{
    static_assert(
        is_const_buffer_sequence<
            ConstBufferSequence>::value,
        "Type requirements not met");

    results rv;
    constexpr int SmallArraySize = 16;
    const_buffer tmp[SmallArraySize];
    auto const tmp_end =
        tmp + SmallArraySize;
    auto it = (begin)(src);
    auto const end_ = (end)(src);
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
        rv = write(
            tmp,
            p - tmp,
            it != end_ || more);
        rv.bytes += saved;
        if(rv.ec.failed())
            return rv;
    }
    return rv;
}

} // buffers
} // boost

#endif
