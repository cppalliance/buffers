//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/http_proto
//

#ifndef BOOST_BUFFERS_BUFFER_HPP
#define BOOST_BUFFERS_BUFFER_HPP

#include <boost/buffers/detail/config.hpp>
#include <boost/buffers/const_buffer.hpp>
#include <boost/buffers/mutable_buffer.hpp>
#include <boost/buffers/type_traits.hpp>
#include <boost/assert.hpp>
#include <cstring>
#include <utility>

namespace boost {
namespace buffers {

/** Copy buffer contents
*/
template<
    class MutableBuffers,
    class ConstBuffers>
std::size_t
buffer_copy(
    MutableBuffers const& to,
    ConstBuffers const& from,
    std::size_t at_most =
        std::size_t(-1)) noexcept
{
    // If you get a compile error here it
    // means that one or both of your types
    // do not meet the requirements.
    static_assert(
        is_mutable_buffers<MutableBuffers>::value,
        "Type requirements not met");
    static_assert(
        is_const_buffers<ConstBuffers>::value,
        "Type requirements not met");

    std::size_t total = 0;
    std::size_t pos0 = 0;
    std::size_t pos1 = 0;
    auto const end0 = from.end();
    auto const end1 = to.end();
    auto it0 = from.begin();
    auto it1 = to.begin();
    while(
        total < at_most &&
        it0 != end0 &&
        it1 != end1)
    {
        const_buffer b0 =
            const_buffer(*it0) + pos0;
        mutable_buffer b1 =
            mutable_buffer(*it1) + pos1;
        std::size_t amount =
        [&]
        {
            std::size_t n = b0.size();
            if( n > b1.size())
                n = b1.size();
            if( n > at_most - total)
                n = at_most - total;
            std::memcpy(
                b1.data(),
                b0.data(),
                n);
            return n;
        }();
        total += amount;
        if(amount == b1.size())
        {
            ++it1;
            pos1 = 0;
        }
        else
        {
            pos1 += amount;
        }
        if(amount == b0.size())
        {
            ++it0;
            pos0 = 0;
        }
        else
        {
            pos0 += amount;
        }
    }
    return total;
}

} // buffers
} // boost

#endif
