//
// Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/buffers
//

#ifndef BOOST_BUFFERS_IMPL_FILTER_HPP
#define BOOST_BUFFERS_IMPL_FILTER_HPP

#include <boost/buffers/range.hpp>
#include <boost/buffers/type_traits.hpp>
#include <boost/buffers/detail/except.hpp>

namespace boost {
namespace buffers {

template<
    class MutableBufferSequence,
    class ConstBufferSequence>
auto
filter::
process(
    MutableBufferSequence const& dest,
    ConstBufferSequence const& src,
    bool more) ->
        results
{
    static_assert(
        is_const_buffer_sequence<
            ConstBufferSequence>::value,
        "Type requirements not met");

    static_assert(
        is_mutable_buffer_sequence<
            MutableBufferSequence>::value,
        "Type requirements not met");

    results rv;
    // VFALCO TODO
    return rv;
}

} // buffers
} // boost

#endif
