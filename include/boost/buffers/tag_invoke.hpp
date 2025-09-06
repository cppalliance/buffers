//
// Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/buffers
//

#ifndef BOOST_BUFFERS_TAG_INVOKE_HPP
#define BOOST_BUFFERS_TAG_INVOKE_HPP

#include <boost/buffers/detail/config.hpp>

#include <type_traits> // VFALCO?

namespace boost {
namespace buffers {

/** size tag for tag_invoke.
*/
struct size_tag {};

enum class slice_how
{
    trim_front,
    trim_back,
    keep_front,
    keep_back
};

/** slice tag for tag_invoke.
*/
struct slice_tag {};

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

} // buffers
} // boost

#endif
