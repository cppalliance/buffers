//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/http_proto
//

#ifndef BOOST_BUFFERS_CONST_BUFFER_SPAN_HPP
#define BOOST_BUFFERS_CONST_BUFFER_SPAN_HPP

#include <boost/buffers/detail/config.hpp>
#include <boost/buffers/const_buffer.hpp>
#include <iterator>

namespace boost {
namespace buffers {

/** Holds a span of buffers that cannot be modified.

    Objects of this type meet the requirements
    of <em>ConstBufferSequence</em>.
*/
class const_buffer_span
{
    const_buffer const* p_ = nullptr;
    std::size_t n_ = 0;

public:
    /** The type of buffer.
    */
    using value_type = const_buffer;

    /** The type of iterators returned.
    */
    using const_iterator = value_type const*;

    /** Constructor.
    */
    const_buffer_span() = default;

    /** Constructor.
    */
    const_buffer_span(
        const_buffer const* p,
        std::size_t n) noexcept
        : p_(p)
        , n_(n)
    {
    }

    /** Constructor.
    */
    const_buffer_span(
        const_buffer_span const&) = default;

    /** Assignment.
    */
    const_buffer_span& operator=(
        const_buffer_span const&) = default;

    /** Return an iterator to the beginning.
    */
    const_iterator
    begin() const noexcept
    {
        return p_;
    }

    /** Return an iterator to the end.
    */
    const_iterator
    end() const noexcept
    {
        return p_ + n_;
    }
};

} // buffers
} // boost

#endif
