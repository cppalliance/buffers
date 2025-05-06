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
#include <boost/buffers/const_buffer.hpp>
#include <boost/buffers/mutable_buffer.hpp>
#include <boost/buffers/range.hpp>
#include <boost/buffers/tag_invoke.hpp>
#include <boost/buffers/type_traits.hpp>

#include <boost/buffers/prefix.hpp>
#include <boost/buffers/suffix.hpp>

namespace boost {
namespace buffers {

namespace detail {

struct sans_suffix_impl
{
    template<class BufferSequence>
    prefix_type<BufferSequence>
    operator()(
        BufferSequence const& b,
        std::size_t n) const
    {
        auto const n0 = size(b);
        if(n < n0)
            return prefix(b, n0 - n);
        return prefix(b, 0);
    }
};

} // detail

/** Return a prefix of the buffer sequence.
*/
constexpr detail::sans_suffix_impl sans_suffix{};

} // buffers
} // boost

#endif
