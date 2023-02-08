//
// Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/buffers
//

#ifndef BOOST_BUFFERS_BUFFER_SIZE_HPP
#define BOOST_BUFFERS_BUFFER_SIZE_HPP

#include <boost/buffers/detail/config.hpp>
#include <boost/buffers/const_buffer.hpp>
#include <boost/buffers/tag_invoke.hpp>
#include <boost/buffers/type_traits.hpp>

namespace boost {
namespace buffers {

#ifdef BOOST_BUFFERS_DOCS

/** Return the total octets in a buffer sequence

    @par Constraints
    @code
    is_const_buffer_sequence< ConstBufferSequence >::value == true
    @endcode
*/
template<
    class ConstBufferSequence>
std::size_t
buffer_size(
    ConstBufferSequence const& b) noexcept;

#else

template<class Buffers>
std::size_t
tag_invoke(
    size_tag const&,
    Buffers const& bs) noexcept
{
    std::size_t n = 0;
    for(const_buffer b : bs)
        n += b.size();
    return n;
}

namespace detail {

struct buffer_size_t
{
    template<
        class Buffers,
        class = typename std::enable_if<
            is_const_buffer_sequence<Buffers>::value
        >::type>
    std::size_t
    operator()(
        Buffers const& bs) const noexcept
    {
        return tag_invoke(size_tag{}, bs);
    }
};

} // detail

constexpr detail::buffer_size_t buffer_size{};

#endif

} // buffers
} // boost

#endif
