//
// Copyright (c) 2025 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/buffers
//

#include <boost/buffers/buffer.hpp>
#include <boost/core/span.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/static_assert.hpp>

#include <type_traits>

#include "test_buffers.hpp"

namespace boost {
namespace buffers {

// These tests check to make sure the Asio buffer
// sequences and our buffer sequences are interoperable

// our buffer is convertible to asio buffer
BOOST_STATIC_ASSERT(  std::is_convertible<const_buffer,   asio::const_buffer>::value);
BOOST_STATIC_ASSERT(! std::is_convertible<const_buffer,   asio::mutable_buffer>::value);
BOOST_STATIC_ASSERT(  std::is_convertible<mutable_buffer, asio::const_buffer>::value);
BOOST_STATIC_ASSERT(  std::is_convertible<mutable_buffer, asio::mutable_buffer>::value);

// asio buffer is convertible to our buffer
//BOOST_STATIC_ASSERT(  std::is_convertible<asio::const_buffer,   const_buffer>::value);
//BOOST_STATIC_ASSERT(! std::is_convertible<asio::const_buffer,   mutable_buffer>::value);
//BOOST_STATIC_ASSERT(  std::is_convertible<asio::mutable_buffer, const_buffer>::value);
//BOOST_STATIC_ASSERT(  std::is_convertible<asio::mutable_buffer, mutable_buffer>::value);

// span of asio buffer is an asio sequence
BOOST_STATIC_ASSERT(  asio::is_const_buffer_sequence<  span<asio::const_buffer>>::value);
BOOST_STATIC_ASSERT(  asio::is_const_buffer_sequence<  span<asio::mutable_buffer>>::value);
BOOST_STATIC_ASSERT(! asio::is_mutable_buffer_sequence<span<asio::const_buffer>>::value);
BOOST_STATIC_ASSERT(  asio::is_mutable_buffer_sequence<span<asio::mutable_buffer>>::value);

// span of our buffer is an asio sequence
BOOST_STATIC_ASSERT(  asio::is_const_buffer_sequence<span<const_buffer>>::value);
BOOST_STATIC_ASSERT(  asio::is_const_buffer_sequence<span<mutable_buffer>>::value);
BOOST_STATIC_ASSERT(! asio::is_mutable_buffer_sequence<span<const_buffer>>::value);
BOOST_STATIC_ASSERT(  asio::is_mutable_buffer_sequence<span<mutable_buffer>>::value);

// span of asio buffer is our sequence
//BOOST_STATIC_ASSERT(  is_const_buffer_sequence<  span<asio::const_buffer const>>::value);
//BOOST_STATIC_ASSERT(  is_const_buffer_sequence<  span<asio::mutable_buffer const>>::value);
//BOOST_STATIC_ASSERT(! is_mutable_buffer_sequence<span<asio::const_buffer const>>::value);
//BOOST_STATIC_ASSERT(  is_mutable_buffer_sequence<span<asio::mutable_buffer const>>::value);

// span of our buffer is our sequence
BOOST_STATIC_ASSERT(  is_const_buffer_sequence<  span<const_buffer const>>::value);
BOOST_STATIC_ASSERT(  is_const_buffer_sequence<  span<mutable_buffer const>>::value);
BOOST_STATIC_ASSERT(! is_mutable_buffer_sequence<span<const_buffer const>>::value);
BOOST_STATIC_ASSERT(  is_mutable_buffer_sequence<span<mutable_buffer const>>::value);

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
