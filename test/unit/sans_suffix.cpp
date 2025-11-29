//
// Copyright (c) 2025 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/buffers
//

// Test that header file is self-contained.
#include <boost/buffers/sans_suffix.hpp>

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

struct sans_suffix_test
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
        // Test sans_suffix with const_buffer_pair
        std::string s = "Hello, World!";
        const_buffer_pair bp{{
            const_buffer(s.data(), 5),
            const_buffer(s.data() + 5, s.size() - 5) }};

        // Skip last 7 bytes
        auto bs = sans_suffix(bp, 7);
        check(bs, "Hello,");

        // Skip 0 bytes
        auto bs2 = sans_suffix(bp, 0);
        check(bs2, s);

        // Skip more than available
        auto bs3 = sans_suffix(bp, 100);
        BOOST_TEST_EQ(size(bs3), 0);
    }
};

TEST_SUITE(
    sans_suffix_test,
    "boost.buffers.sans_suffix");

} // buffers
} // boost
