//
// Copyright (c) 2025 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/buffers
//

#ifndef BOOST_BUFFERS_SANS_PREFIX_HPP
#define BOOST_BUFFERS_SANS_PREFIX_HPP

#include <boost/buffers/detail/config.hpp>
#include <boost/buffers/slice.hpp>
#include <boost/buffers/remove_prefix.hpp>
#include <type_traits>

namespace boost {
namespace buffers {

template<class T> class slice_of;

namespace detail {

template<class T, class = void>
struct has_tag_invoke : std::false_type {};

template<class T>
struct has_tag_invoke<T, decltype(tag_invoke(
    std::declval<slice_tag const&>(),
    std::declval<T&>(),
    std::declval<slice_how>(),
    std::declval<std::size_t>()))>
    : std::true_type {};

} // detail

/** Alias for the type representing a slice of T
*/
template<class T>
using slice_type = typename std::conditional<
    detail::has_tag_invoke<T>::value,
    T, slice_of<T> >::type;

/** Return a sequence representing all but the first `n` bytes of a buffer sequence

    @param bs The buffer sequence.

    @param n The number of bytes to skip.

    @return The sequence representing all but the first `n` bytes.
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
        remove_prefix(result, n);
        return result;
    }
} sans_prefix{};

} // buffers
} // boost

#endif
