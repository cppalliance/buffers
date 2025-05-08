//
// Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/buffers
//

// Test that header file is self-contained.
#include <boost/buffers/const_buffer.hpp>

#include "test_helpers.hpp"

namespace boost {
namespace buffers {

struct const_buffer_test
{
    void
    testMembers()
    {
        // const_buffer()
        BOOST_TEST_EQ(const_buffer().size(), 0);
        BOOST_TEST_EQ(const_buffer().data(), nullptr);

        // const_buffer(void const*, size_t)
        {
            char const* p = "12345";
            const_buffer b( p, 5 );
            BOOST_TEST_EQ(b.data(), p);
            BOOST_TEST_EQ(b.size(), 5);
        }

        // const_buffer(const_buffer)
        {
            char const* p = "12345";
            const_buffer b0( p, 5 );
            const_buffer b(b0);
            BOOST_TEST_EQ(b.data(), p);
            BOOST_TEST_EQ(b.size(), 5);
        }

        // const_buffer(mutable_buffer)
        {
            char buf[6] = "12345";
            mutable_buffer b0( buf, 5 );
            const_buffer b(b0);
            BOOST_TEST_EQ(b.data(), buf);
            BOOST_TEST_EQ(b.size(), 5);
        }

        // operator=(const_buffer)
        {
            char const* p = "12345";
            const_buffer b;
            b = const_buffer(p, 5);
            BOOST_TEST_EQ(b.data(), p);
            BOOST_TEST_EQ(b.size(), 5);
        }
    }

    void
    testBuffer()
    {
        auto const& pat = test_pattern();
        const_buffer cb(&pat[0], pat.size());
        test_buffer_sequence(cb);
    }

    void
    run()
    {
        testMembers();
        testBuffer();
    }
};

TEST_SUITE(
    const_buffer_test,
    "boost.buffers.const_buffer");

} // buffers
} // boost
