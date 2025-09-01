//
// Copyright (c) 2025 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/buffers
//

#include <boost/buffers/const_buffer.hpp>
#include <boost/buffers/mutable_buffer.hpp>

#include <boost/asio/buffer.hpp>
#include <boost/static_assert.hpp>

#include <type_traits>

#include "test_buffers.hpp"

namespace boost {
namespace buffers {

// asio buffers constructing from ours

#if 0
BOOST_STATIC_ASSERT(
    asio::detail::has_subspan_memfn<const_buffer>::value);

BOOST_STATIC_ASSERT(
    std::is_convertible<
        const_buffer,
        asio::const_buffer>::value);

BOOST_STATIC_ASSERT(
    std::is_constructible<
        asio::const_buffer,
        mutable_buffer>::value);

BOOST_STATIC_ASSERT(
    std::is_constructible<
        asio::mutable_buffer,
        mutable_buffer>::value);

BOOST_STATIC_ASSERT(
    ! std::is_constructible<
        asio::mutable_buffer,
        const_buffer>::value);
#endif

struct asio_test
{
    void
    run()
    {
    }
};

TEST_SUITE(
    asio_test,
    "boost.buffers.asio");

} // buffers
} // boost
