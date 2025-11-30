//
// Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/buffers
//

#ifndef BOOST_BUFFERS_FRONT_HPP
#define BOOST_BUFFERS_FRONT_HPP

#include <boost/buffers/detail/config.hpp>
#include <boost/buffers/buffer.hpp>
#include <boost/buffers/range.hpp>
#include <type_traits>

namespace boost {
namespace buffers {

/** Return the first buffer in a sequence.
*/
constexpr struct front_mrdocs_workaround_t
{
    template<
        class MutableBufferSequence
        , class = typename std::enable_if<
            is_mutable_buffer_sequence<
                MutableBufferSequence>::value
        >::type>
    mutable_buffer
    operator()(
        MutableBufferSequence const& bs) const noexcept
    {
        auto const it = begin(bs);
        if(it != end(bs))
            return *it;
        return {};
    }

    template<
        class ConstBufferSequence
        , class = typename std::enable_if<
            ! is_mutable_buffer_sequence<
                ConstBufferSequence>::value
        >::type>
    const_buffer
    operator()(
        ConstBufferSequence const& bs) const noexcept
    {
        static_assert(
            is_const_buffer_sequence<
                ConstBufferSequence>::value,
            "Type requirements not met");

        auto const it = bs.begin();
        if(it != bs.end())
            return *it;
        return {};
    }
} const front{};

} // buffers
} // boost

#endif
