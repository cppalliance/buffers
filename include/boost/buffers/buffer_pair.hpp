//
// Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/buffers
//

#ifndef BOOST_BUFFERS_BUFFER_PAIR_HPP
#define BOOST_BUFFERS_BUFFER_PAIR_HPP

#include <boost/buffers/detail/config.hpp>
#include <boost/buffers/buffer.hpp>
#include <array>

namespace boost {
namespace buffers {

/** A constant buffer pair
*/
using const_buffer_pair = std::array<const_buffer,2>;

BOOST_BUFFERS_DECL
void
tag_invoke(
    slice_tag const&,
    const_buffer_pair& bs,
    slice_how how,
    std::size_t n) noexcept;

/** A mutable buffer pair
*/
using mutable_buffer_pair = std::array<mutable_buffer,2>;

BOOST_BUFFERS_DECL
void
tag_invoke(
    slice_tag const&,
    mutable_buffer_pair& bs,
    slice_how how,
    std::size_t n) noexcept;

} // buffers
} // boost

#endif
