//
// Copyright (c) 2025 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/buffers
//

// Test that header file is self-contained.
#include <boost/buffers/remove_prefix.hpp>

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

struct remove_prefix_test
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

    using seq_type = std::array<const_buffer, 3>;

    void
    grind(
        slice_of<seq_type> const& bs0,
        core::string_view pat0)
    {
        auto const n = size(bs0);
        if(! BOOST_TEST_EQ(n, pat0.size()))
            return;
        for(std::size_t i = 0; i < n; ++i)
        {
            auto bs = bs0;
            auto pat = pat0.substr(i);
            remove_prefix(bs, i);
            check(bs, pat);
        }
        // n >= size
        for(std::size_t i = 0; i < 2; ++i)
        {
            auto bs = bs0;
            remove_prefix(bs, n + i);
            BOOST_TEST_EQ(size(bs), 0);
            check(bs, "");
        }
    }

    void
    run()
    {
        // Test remove_prefix with slice_of
        std::string s = "Hello, World!";
        const_buffer_pair bp{{
            const_buffer(s.data(), 5),
            const_buffer(s.data() + 5, s.size() - 5) }};

        slice_of<const_buffer_pair> bs(bp);
        BOOST_TEST_EQ(size(bs), s.size());

        // Remove first 7 bytes
        remove_prefix(bs, 7);
        check(bs, "World!");

        // Remove 0 bytes
        slice_of<const_buffer_pair> bs2(bp);
        remove_prefix(bs2, 0);
        check(bs2, s);

        // Remove more than available
        slice_of<const_buffer_pair> bs3(bp);
        remove_prefix(bs3, 100);
        BOOST_TEST_EQ(size(bs3), 0);
    }
};

TEST_SUITE(
    remove_prefix_test,
    "boost.buffers.remove_prefix");

} // buffers
} // boost
