//
// Copyright (c) 2025 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/buffers
//

// Test that header file is self-contained.
#include <boost/buffers/any_buffers.hpp>

#include <boost/buffers/buffer_pair.hpp>
#include <boost/buffers/slice.hpp>
#include <boost/buffers/to_string.hpp>

#include <boost/core/detail/static_assert.hpp>
#include <boost/core/detail/string_view.hpp>

#include "test_suite.hpp"

namespace boost {
namespace buffers {

BOOST_CORE_STATIC_ASSERT(  is_const_buffer_sequence<any_buffers<true>>::value);
BOOST_CORE_STATIC_ASSERT(  is_const_buffer_sequence<any_buffers<false>>::value);
BOOST_CORE_STATIC_ASSERT(! is_mutable_buffer_sequence<any_buffers<true>>::value);
BOOST_CORE_STATIC_ASSERT(  is_mutable_buffer_sequence<any_buffers<false>>::value);

namespace {

} // (anon)

struct any_buffers_test
{
    void run()
    {
        core::string_view s0 = "Hello, world!";
        core::string_view s1 = "Goodbye, wg21!";

        // const_buffer
        {
            const_buffer bs(s0.data(), s0.size());
            any_buffers<true> ab(bs);
            BOOST_TEST_EQ(to_string(ab), to_string(bs)); 
            BOOST_TEST_EQ(to_string(make_any_buffers(bs)), to_string(bs));
        }

        // mutable_buffer
        {
            const_buffer bs(s0.data(), s0.size());
            any_buffers<true> ab(bs);
            BOOST_TEST_EQ(to_string(ab), to_string(bs));
            BOOST_TEST_EQ(to_string(make_any_buffers(bs)), to_string(bs));
        }

        // const_buffer_pair
        {
            const_buffer_pair bs({{
                { s0.data(), s0.size() },
                { s1.data(), s1.size() } }});
            any_buffers<true> ab(bs);
            BOOST_TEST_EQ(to_string(ab), to_string(bs));
            BOOST_TEST_EQ(to_string(make_any_buffers(bs)), to_string(bs));
        }

        // slice (big iterators)
        {
            const_buffer_pair bs({{
                { s0.data(), s0.size() },
                { s1.data(), s1.size() } }});
            slice_of<const_buffer_pair> cb(bs);
            any_buffers<true> ab(cb);
            BOOST_TEST_EQ(to_string(ab), to_string(bs));
            BOOST_TEST_EQ(to_string(make_any_buffers(bs)), to_string(bs));
        }
    }
};

TEST_SUITE(
    any_buffers_test,
    "boost.buffers.any_buffers");

} // buffers
} // boost
