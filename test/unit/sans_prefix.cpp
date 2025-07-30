//
// Copyright (c) 2025 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/buffers
//

// Test that header file is self-contained.
#include <boost/buffers/sans_prefix.hpp>

#include "test_suite.hpp"

namespace boost {
namespace buffers {

struct sans_prefix_test
{
    void
    run()
    {
    }
};

TEST_SUITE(
    sans_prefix_test,
    "boost.buffers.sans_prefix");

} // buffers
} // boost
