//
// Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/buffers
//

#ifndef BOOST_BUFFERS_BUFFER_SIZE_HPP
#define BOOST_BUFFERS_BUFFER_SIZE_HPP

#include <boost/buffers/detail/config.hpp>
#include <boost/buffers/const_buffer.hpp>
#include <boost/buffers/range.hpp>
#include <boost/buffers/tag_invoke.hpp>
#include <boost/buffers/type_traits.hpp>

namespace boost {
namespace buffers {

template<class Buffers>
std::size_t
tag_invoke(
    size_tag const&,
    Buffers const& bs) noexcept
{
    std::size_t n = 0;
    for(const_buffer b : range(bs))
        n += b.size();
    return n;
}

namespace detail {

struct buffer_size_impl
{
    template<class Buffers>
    std::size_t
    operator()(
        Buffers const& bs) const noexcept
    {
        // If you get a compile error here it
        // means that your type does not meet
        // the requirements.
        static_assert(
            is_const_buffer_sequence<Buffers>::value,
            "Type requirements not met.");

        return tag_invoke(size_tag{}, bs);
    }
};

} // detail

/** Return the total octets in a buffer sequence

    @par Constraints
    @code
    is_const_buffer_sequence< ConstBufferSequence >::value == true
    @endcode
*/
constexpr detail::buffer_size_impl buffer_size{};

} // buffers
} // boost

#endif
