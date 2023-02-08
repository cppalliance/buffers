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
read_impl(
    mutable_buffer_span bs) ->
        results
{
    results rv;
    auto it = bs.begin();
    auto const end = bs.end();
    std::size_t n0 = 0;
    for(;;)
    {
        // on_read is always
        // called at least once
        rv += on_read(bs);
        if(rv.ec.failed())
            return rv;
        if(rv.finished)
            goto done;
        auto n = rv.bytes - n0;
        // skip filled buffers
        do
        {
            if(it->size() == 0)
            {
                // 0 sized buffer
                ++it;
                if(it == end)
                    goto done;
                continue;
            }
            if(n < it->size())
            {
                // partial fill
                goto done;
            }
            // filled buffer
            n -= it->size();
            ++it;
            if(it == end)
            {
                // on_read returned
                // too many bytes
                BOOST_ASSERT(n == 0);
                goto done;
            }
        }
        while(n > 0);
        bs = mutable_buffer_span(
            it, end - it);
    }
done:
    return rv;
}

} // buffers
} // boost

#endif
