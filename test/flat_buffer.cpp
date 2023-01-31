//
// Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/buffers
//

// Test that header file is self-contained.
#include <boost/buffers/flat_buffer.hpp>

#include <boost/buffers/type_traits.hpp>
#include <boost/static_assert.hpp>

#include "test_suite.hpp"

namespace boost {
namespace buffers {

BOOST_STATIC_ASSERT(
    is_dynamic_buffer<flat_buffer>::value);

struct flat_buffer_test
{
    void
    run()
    {
    }
};

TEST_SUITE(
    flat_buffer_test,
    "boost.buffers.flat_buffer");

} // buffers
} // boost
