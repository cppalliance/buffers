//
// Copyright (c) 2025 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/beast2
//

// Test that header file is self-contained.
#include <boost/buffers/any_stream.hpp>

#ifdef BOOST_BUFFERS_HAS_CORO

#include "test_suite.hpp"

namespace boost {
namespace buffers {

struct any_stream_test
{
    void run()
    {
    }
};

TEST_SUITE(
    any_stream_test,
    "boost.buffers.any_stream");

} // buffers
} // boost

#endif
