//
// Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/buffers
//

#ifndef BOOST_BUFFERS_MUTABLE_BUFFER_SPAN_HPP
#define BOOST_BUFFERS_MUTABLE_BUFFER_SPAN_HPP

#include <boost/buffers/detail/config.hpp>
#include <boost/buffers/mutable_buffer.hpp>
#include <boost/buffers/tag_invoke.hpp>
#include <boost/buffers/type_traits.hpp>

namespace boost {
namespace buffers {

/** Holds a span of buffers that are modifiable.

    Objects of this type meet the requirements
    of <em>MutableBufferSequence</em>.
*/
class mutable_buffer_span
{
    mutable_buffer const* p_ = nullptr;
    std::size_t n_ = 0;

    friend class mutable_buffer_subspan;

public:
    /** The type of buffer.
    */
    using value_type = mutable_buffer;

    /** The type of iterators returned.
    */
    using const_iterator = value_type const*;

    /** Constructor.
    */
    mutable_buffer_span() = default;

    /** Constructor.
    */
    mutable_buffer_span(
        mutable_buffer const* p,
        std::size_t n) noexcept
        : p_(p)
        , n_(n)
    {
    }

    /** Constructor.
    */
    template<
        class MutableBufferSequence
        , class = typename std::enable_if<
            ! std::is_same<
                MutableBufferSequence,
                mutable_buffer_span>::value &&
            is_mutable_buffer_sequence<
                MutableBufferSequence>::value &&
            std::is_same<decltype(
                std::declval<MutableBufferSequence
                    const&>().begin()),
                mutable_buffer const*>::value
            >::type
    >
    explicit
    mutable_buffer_span(
        MutableBufferSequence const& bs) noexcept
        : p_(bs.begin())
        , n_(bs.end() - bs.begin())
    {
    }

    /** Constructor.
    */
    inline
    mutable_buffer_span(
        mutable_buffer_span const&) = default;

    /** Assignment.
    */
    mutable_buffer_span& operator=(
        mutable_buffer_span const&) = default;

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
