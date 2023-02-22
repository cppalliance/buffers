//
// Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/buffers
//

#ifndef BOOST_BUFFERS_IMPL_FILTER_IPP
#define BOOST_BUFFERS_IMPL_FILTER_IPP

#include <boost/buffers/filter.hpp>
#include <boost/buffers/range.hpp>

namespace boost {
namespace buffers {

auto
filter::
process(
    mutable_buffer_span const& out,
    const_buffer_span const& in,
    bool more) ->
        results
{
    results rv;
    auto it0 = begin(in);
    auto it1 = begin(out);
    auto const end0 = end(in);
    auto const end1 = end(out);
    while(it1 != end1)
    {
        ++it1;
        (void)more;
        (void)end0;
        (void)it0;
    }
    return rv;
}

} // buffers
} // boost

#endif
