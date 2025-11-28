//
// Copyright (c) 2025 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/buffers
//

#ifndef BOOST_BUFFERS_TO_STRING_HPP
#define BOOST_BUFFERS_TO_STRING_HPP

#include <boost/buffers/detail/config.hpp>
#include <boost/buffers/buffer.hpp>
#include <boost/core/detail/static_assert.hpp>
#include <string>

namespace boost {
namespace buffers {

/** Convert a buffer sequence to a string
    This function constructs a string from the bytes in the
    buffer sequence `bs`.
    @par Constraints
    @code
    requires is_const_buffer_sequence<BufferSequence>::value
    @endcode
    @param bs The buffer sequence
    @return A string holding the bytes from the buffer sequence
*/
template< class ConstBufferSequence >
std::string
to_string(ConstBufferSequence const& bs)
{
    BOOST_CORE_STATIC_ASSERT(
        is_const_buffer_sequence<ConstBufferSequence>::value);
    std::string s;
    auto const e = end(bs);
    for(auto it = begin(bs); it != e; ++it)
    {
        const_buffer b(*it);
        s.append(
            static_cast<char const*>(b.data()),
            b.size());
    }
    return s;
}

} // buffers
} // boost

#endif
