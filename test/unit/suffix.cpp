//
// Copyright (c) 2025 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/buffers
//

// Test that header file is self-contained.
#include <boost/buffers/suffix.hpp>

#include <boost/core/span.hpp>

#include "test_suite.hpp"

namespace boost {
namespace buffers {

struct suffix_test
{
    void
    run()
    {
        // suffix
        {
            char buf[32];
            span<char> s(buf);
            BOOST_TEST(suffix(s, 0).size() == 0);
            BOOST_TEST(suffix(s, 1).size() == 1);
            BOOST_TEST(suffix(s, 99).size() == 32);
        }
    }
};

TEST_SUITE(
    suffix_test,
    "boost.buffers.suffix");

} // buffers
} // boost
