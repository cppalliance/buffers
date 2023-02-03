//
// Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/buffers
//

// Test that header file is self-contained.
#include <boost/buffers/buffered_base.hpp>

#include "test_suite.hpp"

namespace boost {
namespace buffers {

struct buffered_base_test
{
    void
    run()
    {
    }
};

TEST_SUITE(
    buffered_base_test,
    "boost.buffers.buffered_base");

} // buffers
} // boost
