//
// Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/buffers
//

#ifndef BOOST_BUFFERS_SANS_SUFFIX_HPP
#define BOOST_BUFFERS_SANS_SUFFIX_HPP

#include <boost/buffers/detail/config.hpp>
#include <boost/buffers/size.hpp>
#include <boost/buffers/prefix.hpp>

namespace boost {
namespace buffers {

/** Return a prefix of the buffer sequence.
*/
constexpr struct
{
    template<class T>
    constexpr auto operator()(
        T const& bs,
        std::size_t n) const ->
            prefix_type<T>
    {
        return invoke(bs, n, size(bs));
    }

private:
    template<class T>
    constexpr auto invoke(
        T const& bs,
        std::size_t n,
        std::size_t n0) const ->
            prefix_type<T>
    {
        return prefix(bs, (n < n0) ? n0 - n : 0);
    }
} const sans_suffix{};

} // buffers
} // boost

#endif
