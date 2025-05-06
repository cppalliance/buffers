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
#include <boost/buffers/type_traits.hpp>

namespace boost {
namespace buffers {

namespace detail {

struct front_impl
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
};

} // detail

/** Return the first buffer in a sequence.
*/
constexpr detail::front_impl front{};

} // buffers
} // boost

#endif
