//
// Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/buffers
//

#ifndef BOOST_BUFFERS_SANS_PREFIX_HPP
#define BOOST_BUFFERS_SANS_PREFIX_HPP

#include <boost/buffers/detail/config.hpp>
#include <boost/buffers/size.hpp>
#include <boost/buffers/suffix.hpp>

namespace boost {
namespace buffers {

/** Return a suffix of the buffer sequence.
*/
constexpr struct
{
    template<class T>
    constexpr auto operator()(
        T const& bs,
        std::size_t n) const ->
            suffix_type<T>
    {
        return invoke(bs, n, size(bs));
    }

private:
    template<class T>
    constexpr auto invoke(
        T const& bs,
        std::size_t n,
        std::size_t n0) const ->
            suffix_type<T>
    {
        return suffix(bs, (n < n0) ? n0 - n : 0);
    }  
} const sans_prefix{};

} // buffers
} // boost

#endif
