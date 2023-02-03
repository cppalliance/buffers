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
write(
    const_buffer const* src,
    std::size_t src_len,
    bool more) ->
        results
{
    results rv;
    if(src_len == 0)
        return rv;
    if(src_len > 1)
    {
        while(--src_len)
        {
            const_buffer cb(*src++);
            auto saved = rv.bytes;
            rv = write_one(
                cb.data(),
                cb.size(),
                true);
            rv.bytes += saved;
            if(rv.ec.failed())
                return rv;
        }
    }
    const_buffer cb(*src);
    auto saved = rv.bytes;
    rv = write_one(
        cb.data(),
        cb.size(),
        ! more);
    rv.bytes += saved;
    if(rv.ec.failed())
        return rv;
    return rv;
}

} // buffers
} // boost

#endif
