//
// Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/buffers
//

#ifndef BOOST_BUFFERS_MAKE_BUFFER_HPP
#define BOOST_BUFFERS_MAKE_BUFFER_HPP

#include <boost/buffers/detail/config.hpp>
#include <boost/buffers/const_buffer.hpp>
#include <boost/buffers/mutable_buffer.hpp>
#include <boost/buffers/type_traits.hpp>
#include <boost/core/data.hpp>
#include <cstdlib>
#include <type_traits>

namespace boost {
namespace buffers {

/** Return a buffer.
*/
inline
mutable_buffer
make_buffer(
    mutable_buffer const& b) noexcept
{
    return b;
}

/** Return a buffer.
*/
inline
mutable_buffer
make_buffer(
    void* data,
    std::size_t size) noexcept
{
    return mutable_buffer(data, size);
}

/** Return a buffer.
*/
inline
const_buffer
make_buffer(
    const_buffer const& b) noexcept
{
    return b;
}

/** Return a buffer.
*/
inline
const_buffer
make_buffer(
    void const* data,
    std::size_t size) noexcept
{
    return const_buffer(data, size);
}

/** Return a buffer.
*/
template<
    class T, std::size_t N
#ifndef BOOST_BUFFERS_DOCS
    , class = typename std::enable_if<
        std::is_trivially_copyable<T>::value>::type
#endif
>
mutable_buffer
make_buffer(
    T (&data)[N]) noexcept
{
    return mutable_buffer(
        data, N * sizeof(T));
}

/** Return a buffer.
*/
template<
    class T, std::size_t N
#ifndef BOOST_BUFFERS_DOCS
    , class = typename std::enable_if<
        std::is_trivially_copyable<T>::value>::type
#endif
>
const_buffer
make_buffer(
    T const (&data)[N]) noexcept
{
    return const_buffer(
        data, N * sizeof(T));
}

/** Return a buffer.
*/
template<
    class T
#ifndef BOOST_BUFFERS_DOCS
    , typename std::enable_if<
        detail::is_mutable_contiguous_container<T>::value,
        int>::type = 0
#endif
>
mutable_buffer
make_buffer(T& data)
{
    return mutable_buffer(
        boost::data(data),
        sizeof(*(data.data())) * data.size());
}

/** Return a buffer.
*/
template<
    class T
#ifndef BOOST_BUFFERS_DOCS
    , typename std::enable_if<
        detail::is_const_contiguous_container<T>::value,
        int>::type = 0
#endif
>
const_buffer
make_buffer(T& data)
{
    return const_buffer(
        boost::data(data),
        sizeof(*(data.data())) * data.size());
}

} // buffers
} // boost

#endif
