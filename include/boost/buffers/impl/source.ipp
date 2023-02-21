//
// Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/buffers
//

#ifndef BOOST_BUFFERS_IMPL_SOURCE_IPP
#define BOOST_BUFFERS_IMPL_SOURCE_IPP

#include <boost/buffers/source.hpp>
#include <boost/buffers/mutable_buffer.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace buffers {

auto
source::
on_read(
    mutable_buffer_span bs) ->
        results
{
    auto it = bs.begin();
    auto const end_ = bs.end();
    if(it != end_)
    {
        results rv;
        do
        {
            mutable_buffer b(*it++);
            rv += on_read(b);
            if(rv.ec.failed())
                return rv;
            if(rv.finished)
                break;
        }
        while(it != end_);
        return rv;
    }

    // call on_read at least once
    return on_read(
        mutable_buffer{});
}

} // buffers
} // boost

#endif
