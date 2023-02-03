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

namespace boost {
namespace buffers {

auto
source::
read(
    mutable_buffer const* dest,
    std::size_t dest_len) ->
        results
{
    results rv;
    while(dest_len--)
    {
        mutable_buffer mb(*dest++);
        auto saved = rv.bytes;
        rv = read_one(
            mb.data(), mb.size());
        rv.bytes += saved;
        if(rv.ec.failed())
            return rv;
        if(! rv.finished)
            break;
    }
    return rv;
}

} // buffers
} // boost

#endif
