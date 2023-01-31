//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/http_proto
//

#ifndef BOOST_BUFFERS_BUFFER_PAIR_HPP
#define BOOST_BUFFERS_BUFFER_PAIR_HPP

#include <boost/buffers/detail/config.hpp>
#include <boost/buffers/const_buffer.hpp>
#include <boost/buffers/mutable_buffer.hpp>
#include <boost/buffers/type_traits.hpp>
#include <boost/assert.hpp>
#include <boost/config/workaround.hpp>
#include <cstring>
#include <utility>

namespace boost {
namespace buffers {

#if BOOST_WORKAROUND(BOOST_MSVC, < 1910)
# pragma warning (push)
# pragma warning (disable: 4521) // multiple copy constructors specified
# pragma warning (disable: 4522) // multiple assignment operators specified
#endif

template<bool isConst>
class buffer_pair
{
public:
    // VFALCO: This type is public otherwise
    //         asio::buffers_iterator won't compile.
    using value_type = typename
        std::conditional<isConst,
            const_buffer,
            mutable_buffer>::type;

    using const_iterator = value_type const*;

    buffer_pair() = default;

#if defined(BOOST_HTTP_PROTO_DOCS) || ( \
        ! BOOST_WORKAROUND(BOOST_MSVC, < 1910))
    buffer_pair(
        buffer_pair const& other) = default;
    buffer_pair& operator=(
        buffer_pair const& other) = default;

#else
    buffer_pair(
        buffer_pair const& other) noexcept
        : buffer_pair(
            *other.begin(),
            *(other.begin() + 1))
    {
    }

    buffer_pair&
    operator=(buffer_pair const& other)
    {
        b_[0] = other.b_[0];
        b_[1] = other.b_[1];
        return *this;
    }
#endif

    // const pair construction
    // from mutable mutable pair
    template<
        bool isConst_ = isConst,
        class = typename std::enable_if<
            isConst_>::type>
    buffer_pair(
        buffer_pair<false> const& other)
        : buffer_pair(
            other.b_[0],
            other.b_[1])
    {
    }

    // const pair assignment
    // from mutable mutable pair
    template<
        bool isConst_ = isConst,
        class = typename std::enable_if<
            isConst_>::type>
    buffer_pair&
    operator=(
        buffer_pair<false> const& other)
    {
        b_[0] = other.b_[0];
        b_[1] = other.b_[1];
        return *this;
    }

    buffer_pair(
        value_type b0,
        value_type b1) noexcept
    {
        if(b0.size() > 0)
        {
            b_[0] = b0;
            b_[1] = b1;
        }
        else
        {
            b_[0] = b1;
        }
    }

    const_buffer
    operator[](
        std::size_t i) const noexcept
    {
        BOOST_ASSERT(i < 2);
        return b_[i];
    }

    const_iterator
    begin() const noexcept
    {
        return b_;
    }

    const_iterator
    end() const noexcept
    {
        if(b_[1].size() > 0)
            return &b_[2];
        if(b_[0].size() > 0)
            return &b_[1];
        return b_;
    }

private:
    value_type b_[2];
};

#if BOOST_WORKAROUND(BOOST_MSVC, < 1910)
# pragma warning (pop)
#endif

/** A mutable buffer pair
*/
using mutable_buffer_pair =
    buffer_pair<false>;

/** A const buffer pair
*/
using const_buffer_pair =
    buffer_pair<true>;

} // buffers
} // boost

#endif
