//
// Copyright (c) 2025 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/buffers
//

// Test that header file is self-contained.
#include <boost/buffers/keep_prefix.hpp>

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

struct keep_prefix_test
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
        // Test keep_prefix with slice_of
        std::string s = "Hello, World!";
        const_buffer_pair bp{{
            const_buffer(s.data(), 5),
            const_buffer(s.data() + 5, s.size() - 5) }};

        slice_of<const_buffer_pair> bs(bp);
        BOOST_TEST_EQ(size(bs), s.size());

        // Keep first 5 bytes
        keep_prefix(bs, 5);
        check(bs, "Hello");

        // Keep 0 bytes
        slice_of<const_buffer_pair> bs2(bp);
        keep_prefix(bs2, 0);
        BOOST_TEST_EQ(size(bs2), 0);

        // Keep more than available
        slice_of<const_buffer_pair> bs3(bp);
        keep_prefix(bs3, 100);
        check(bs3, s);
    }
};

TEST_SUITE(
    keep_prefix_test,
    "boost.buffers.keep_prefix");

} // buffers
} // boost
