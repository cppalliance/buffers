//
// Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/buffers
//

// Test that header file is self-contained.
#include <boost/buffers/mutable_buffer.hpp>

#include "test_helpers.hpp"

namespace boost {
namespace buffers {

struct mutable_buffer_test
{
    void
    testMembers()
    {
        // mutable_buffer()
        BOOST_TEST_EQ(mutable_buffer().size(), 0);

        // mutable_buffer(void const*, size_t)
        {
            char p[6] = "12345";
            mutable_buffer b( p, 5 );
            BOOST_TEST_EQ(b.data(), p);
            BOOST_TEST_EQ(b.size(), 5);
        }

        // mutable_buffer(mutable_buffer)
        {
            char p[6] = "12345";
            mutable_buffer b0( p, 5 );
            mutable_buffer b(b0);
            BOOST_TEST_EQ(b.data(), p);
            BOOST_TEST_EQ(b.size(), 5);
        }

        // mutable_buffer(mutable_buffer)
        {
            char buf[6] = "12345";
            mutable_buffer b0( buf, 5 );
            mutable_buffer b(b0);
            BOOST_TEST_EQ(b.data(), buf);
            BOOST_TEST_EQ(b.size(), 5);
        }

        // operator=(mutable_buffer)
        {
            char p[6] = "12345";
            mutable_buffer b;
            b = mutable_buffer(p, 5);
            BOOST_TEST_EQ(b.data(), p);
            BOOST_TEST_EQ(b.size(), 5);
        }

        // VFALCO TODO algorithm tests
        // prefix, suffix, sans_prefix, sans_suffix
    }

    void
    testBuffer()
    {
        std::string s = test_pattern();
        mutable_buffer cb(&s[0], s.size());
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
    mutable_buffer_test,
    "boost.buffers.mutable_buffer");

} // buffers
} // boost
