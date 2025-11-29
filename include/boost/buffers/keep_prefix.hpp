//
// Copyright (c) 2025 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/buffers
//

#ifndef BOOST_BUFFERS_KEEP_PREFIX_HPP
#define BOOST_BUFFERS_KEEP_PREFIX_HPP

#include <boost/buffers/detail/config.hpp>
#include <boost/buffers/detail/type_traits.hpp>
#include <boost/buffers/buffer.hpp>
#include <boost/buffers/range.hpp>
#include <type_traits>

namespace boost {
namespace buffers {

/** Remove all but the first `n` bytes from a buffer sequence

    @param bs The buffer sequence to modify.

    @param n The number of bytes to keep.
*/
constexpr struct
{
    template<class BufferSequence>
    auto operator()(
        BufferSequence& bs,
        std::size_t n) const -> typename std::enable_if<
            is_const_buffer_sequence<BufferSequence>::value &&
            detail::has_tag_invoke<BufferSequence>::value>::type

    {
        tag_invoke(slice_tag{}, bs, slice_how::keep_prefix, n);
    }
} const keep_prefix{};

} // buffers
} // boost

#endif
