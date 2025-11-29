//
// Copyright (c) 2025 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/buffers
//

// Test that header file is self-contained.
#include <boost/buffers/prefix.hpp>

#include <boost/buffers/buffer_pair.hpp>
#include <boost/buffers/copy.hpp>
#include <boost/buffers/make_buffer.hpp>
#include <boost/buffers/slice.hpp>
#include <boost/core/detail/string_view.hpp>
#include <array>

#include "test_buffers.hpp"
#include "test_suite.hpp"

namespace boost {
namespace buffers {

struct prefix_test
{
    template<class B>
    static
    void
    check(
        B const& b,
        core::string_view s)
    {
        auto constexpr M = 1024;
        char buf[M];
        if(! BOOST_TEST_LE(size(b), M))
            return;
        if(! BOOST_TEST_EQ(size(b), s.size()))
            return;
        auto const n = copy(
            mutable_buffer(buf, M), b);
        if(! BOOST_TEST_EQ(n, s.size()))
            return;
        if(! BOOST_TEST_EQ(core::string_view(buf, n), s))
            return;
        test::check_iterators(b, s);
    }

    void
    run()
    {
        // Test prefix with const_buffer_pair
        std::string s = "Hello, World!";
        const_buffer_pair bp{{
            const_buffer(s.data(), 5),
            const_buffer(s.data() + 5, s.size() - 5) }};

        // Get first 5 bytes
        auto bs = prefix(bp, 5);
        check(bs, "Hello");

        // Get 0 bytes
        auto bs2 = prefix(bp, 0);
        BOOST_TEST_EQ(size(bs2), 0);

        // Get more than available
        auto bs3 = prefix(bp, 100);
        check(bs3, s);
    }
};

TEST_SUITE(
    prefix_test,
    "boost.buffers.prefix");

} // buffers
} // boost
