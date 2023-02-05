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

#include <boost/buffers/filter.hpp>

namespace boost {
namespace buffers {

auto
filter::
process(
    mutable_buffer const* dest,
    std::size_t dest_len,
    const_buffer const* src,
    std::size_t src_len,
    bool more) ->
        results
{
    results rv;
    // VFALCO TODO
    (void)dest;
    (void)dest_len;
    (void)src;
    (void)src_len;
    (void)more;
    return rv;
}

} // buffers
} // boost

#endif
