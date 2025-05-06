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

constexpr struct
{
    template<class T>
    constexpr auto operator()(
        T const& bs,
        std::size_t n) const ->
            suffix_type<T>
    {
        auto const n0 = size(bs);
        if(n < n0)
            return suffix(bs, n0 - n);
        return suffix(bs, 0);
    }
} const sans_prefix{};

} // buffers
} // boost

#endif
