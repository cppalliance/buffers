//
// Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/buffers
//

#ifndef BOOST_BUFFERS_COPY_HPP
#define BOOST_BUFFERS_COPY_HPP

#include <boost/buffers/detail/config.hpp>
#include <boost/buffers/range.hpp>
#include <boost/buffers/slice.hpp>
#include <boost/buffers/type_traits.hpp>
#include <boost/assert.hpp>
#include <cstring>
#include <utility>

namespace boost {
namespace buffers {
namespace detail {

struct buffer_copy_impl
{
    template<
        class MutableBuffers,
        class ConstBuffers>
    std::size_t
    operator()(
        MutableBuffers const& to,
        ConstBuffers const& from,
        std::size_t at_most =
            std::size_t(-1)) const noexcept
    {
        // If you get a compile error here it
        // means that one or both of your types
        // do not meet the requirements.
        static_assert(
            is_mutable_buffer_sequence<
                MutableBuffers>::value,
            "Type requirements not met");
        static_assert(
            is_const_buffer_sequence<
                ConstBuffers>::value,
            "Type requirements not met");

        std::size_t total = 0;
        std::size_t pos0 = 0;
        std::size_t pos1 = 0;
        auto const end0 = end(from);
        auto const end1 = end(to);
        auto it0 = begin(from);
        auto it1 = begin(to);
        while(
            total < at_most &&
            it0 != end0 &&
            it1 != end1)
        {
            auto b0 = const_buffer(*it0);
            auto b1 = mutable_buffer(*it1);
            trim_front(b0, pos0);
            trim_front(b1, pos1);
            std::size_t const amount =
            [&]
            {
                std::size_t n = b0.size();
                if( n > b1.size())
                    n = b1.size();
                if( n > at_most - total)
                    n = at_most - total;
                if(n != 0)
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
};

} // detail

/** Copy buffer contents
*/
constexpr detail::buffer_copy_impl copy{};

} // buffers
} // boost

#endif
