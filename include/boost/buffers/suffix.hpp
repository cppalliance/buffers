//
// Copyright (c) 2025 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/buffers
//

#ifndef BOOST_BUFFERS_SUFFIX_HPP
#define BOOST_BUFFERS_SUFFIX_HPP

#include <boost/buffers/detail/config.hpp>
#include <boost/buffers/detail/type_traits.hpp>
#include <boost/buffers/slice.hpp>
#include <boost/buffers/keep_suffix.hpp>
#include <type_traits>

namespace boost {
namespace buffers {

template<class T> class slice_of;

/** Alias for the type representing a slice of T
*/
template<class T>
using slice_type = typename std::conditional<
    detail::has_tag_invoke<T>::value,
    T, slice_of<T> >::type;

/** Return a sequence representing the last `n` bytes of a buffer sequence

    @param bs The buffer sequence.

    @param n The number of bytes.

    @return The sequence representing the last `n` bytes.
*/
constexpr struct
{
    template<class BufferSequence>
    auto operator()(
        BufferSequence const& bs,
        std::size_t n) const noexcept -> typename std::enable_if<
            is_const_buffer_sequence<BufferSequence>::value,
            slice_type<BufferSequence>>::type
    {
        slice_type<BufferSequence> result(bs);
        keep_suffix(result, n);
        return result;
    }
} suffix{};

} // buffers
} // boost

#endif
