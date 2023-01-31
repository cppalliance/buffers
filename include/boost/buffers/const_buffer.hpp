//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/http_proto
//

#ifndef BOOST_BUFFERS_CONST_BUFFER_HPP
#define BOOST_BUFFERS_CONST_BUFFER_HPP

#include <boost/buffers/detail/config.hpp>
#include <boost/buffers/mutable_buffer.hpp>

namespace boost {
namespace buffers {

/** Holds a buffer that cannot be modified.
*/
class const_buffer
{
    unsigned char const* p_ = nullptr;
    std::size_t n_ = 0;

public:
    using value_type = const_buffer;
    using const_iterator =
        value_type const*;

    /** Constructor.
    */
    const_buffer() = default;

    /** Constructor.
    */
    const_buffer(
        const_buffer const&) = default;

    /** Constructor.
    */
    const_buffer(
        mutable_buffer const& b) noexcept
        : p_(static_cast<
            unsigned char const*>(b.data()))
        , n_(b.size())
    {
    }

    /** Constructor.
    */
    const_buffer(
        void const* data,
        std::size_t size) noexcept
        : p_(static_cast<
            unsigned char const*>(data))
        , n_(size)
    {
    }

    /** Assignment.
    */
    const_buffer& operator=(
        const_buffer const&) = default;

#ifndef BOOST_BUFFERS_DOCS
    // conversion to boost::asio::const_buffer
    template<
        class T
        , class = typename std::enable_if<
            std::is_constructible<T,
                void const*, std::size_t>::value &&
            ! std::is_same<T, mutable_buffer>::value &&
            ! std::is_same<T, const_buffer>::value
        >::type
    >
    operator T() const noexcept
    {
        return T{ data(), size() };
    }
#endif

    void const*
    data() const noexcept
    {
        return p_;
    }

    std::size_t
    size() const noexcept
    {
        return n_;
    }

    const_iterator
    begin() const noexcept
    {
        return this;
    }

    const_iterator
    end() const noexcept
    {
        return this + 1;
    }

    /** Remove a prefix from the buffer.
    */
    const_buffer&
    operator+=(std::size_t n) noexcept
    {
        if(n >= n_)
        {
            p_ = p_ + n_;
            n_ = 0;
            return *this;
        }
        p_ = p_ + n;
        n_ -= n;
        return *this;
    }

    /** Return the buffer with a prefix removed.
    */
    friend
    const_buffer
    operator+(
        const_buffer b,
        std::size_t n) noexcept
    {
        return b += n;
    }

    /** Return the buffer with a prefix removed.
    */
    friend
    const_buffer
    operator+(
        std::size_t n,
        const_buffer b) noexcept
    {
        return b += n;
    }

#ifndef BOOST_BUFFERS_DOCS
    friend
    const_buffer
    tag_invoke(
        prefix_tag const&,
        const_buffer const& b,
        std::size_t n) noexcept
    {
        if(n < b.size())
            return { b.p_, n };
        return b;
    }

    friend
    const_buffer
    tag_invoke(
        suffix_tag const&,
        const_buffer const& b,
        std::size_t n) noexcept
    {
        if(n < b.size())
            return { b.p_ + b.n_ - n, n };
        return b;
    }
#endif
};

} // buffers
} // boost

#endif
