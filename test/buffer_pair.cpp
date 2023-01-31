//
// Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/buffers
//

// Test that header file is self-contained.
#include <boost/buffers/buffer_pair.hpp>

#include "test_suite.hpp"

namespace boost {
namespace buffers {

struct buffer_pair_test
{
    void
    run()
    {
    }
};

TEST_SUITE(
    buffer_pair_test,
    "boost.buffers.buffer_pair");

} // http_proto
} // boost
