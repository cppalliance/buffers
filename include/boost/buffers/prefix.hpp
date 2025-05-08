//
// Copyright (c) 2025 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/buffers
//

#ifndef BOOST_BUFFERS_PREFIX_HPP
#define BOOST_BUFFERS_PREFIX_HPP

#include <boost/buffers/detail/config.hpp>
#include <boost/buffers/tag_invoke.hpp>
#include <boost/buffers/detail/is_span.hpp>
#include <type_traits>

namespace boost {
namespace buffers {

/** Alias for the type of a prefix of a buffer sequence.
*/
template<class T>
using prefix_type = decltype(
    tag_invoke(
        std::declval<prefix_tag const&>(),
        std::declval<T const&>(),
        std::declval<std::size_t>()));

// span-like types
template<
    template<class, std::size_t> class Span,
    class T, std::size_t Extent>
constexpr
auto
tag_invoke(
    prefix_tag const&,
    Span<T, Extent> const& bs,
    std::size_t n) ->
        typename std::enable_if<
            detail::is_span<Span<T, Extent>>::value,
            Span<T, Extent>
        >::type
{
    return n <= bs.size() ? bs.subspan(0, n) : bs;
}

/** Return the first n bytes of a buffer sequence
*/
constexpr struct
{
    template<class T>
    constexpr auto operator()(
        T const& bs,
        std::size_t n) const ->
            prefix_type<T>
    {
        return tag_invoke(prefix_tag{}, bs, n);
    }
} const prefix{};

} // buffers
} // boost

#endif
