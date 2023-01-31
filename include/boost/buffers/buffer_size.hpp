//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/http_proto
//

#ifndef BOOST_BUFFERS_BUFFER_SIZE_HPP
#define BOOST_BUFFERS_BUFFER_SIZE_HPP

#include <boost/buffers/detail/config.hpp>
#include <boost/buffers/const_buffer.hpp>
#include <boost/buffers/tag_invoke.hpp>
#include <boost/buffers/type_traits.hpp>

namespace boost {
namespace buffers {

/** Return the total octets in a buffer sequence
*/
template<
    class ConstBuffers
#ifndef BOOST_HTTP_PROTO_DOCS
    , class = typename std::enable_if<
        is_const_buffers<ConstBuffers>::value
    >::type
#endif
>
std::size_t
buffer_size(
    ConstBuffers const& buffers) noexcept
{
    std::size_t n = 0;
    for(const_buffer b : buffers)
        n += b.size();
    return n;
}

} // buffers
} // boost

#endif
