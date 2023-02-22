//
// Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/buffers
//

#ifndef BOOST_BUFFERS_IMPL_SINK_IPP
#define BOOST_BUFFERS_IMPL_SINK_IPP

#include <boost/buffers/sink.hpp>

namespace boost {
namespace buffers {

auto
sink::
on_write(
    const_buffer_span bs,
    bool more) ->
        results
{
    auto it = bs.begin();
    auto const end_ = bs.end();
    results rv;
    if(it == end_)
        return rv;
    do
    {
        const_buffer b(*it++);
        rv += on_write(b,
            it != end_ ||
            more);
        if(rv.ec.failed())
            return rv;
    }
    while(it != end_);
    return rv;
}

} // buffers
} // boost

#endif
