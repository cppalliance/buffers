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

#include <boost/core/span.hpp>

#include "test_buffers.hpp"

/*
#if defined(__cpp_lib_span) && (__cpp_lib_span >= 202002L)
#define HAVE_SPAN 1
#include <span>
#endif
*/

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
    }

    void
    testBuffer()
    {
        std::string pat = test_pattern();
        mutable_buffer cb(&pat[0], pat.size());
        test::check_sequence(cb, pat);
    }

    void
    testSpan()
    {
        {
            BOOST_STATIC_ASSERT(is_mutable_buffer_sequence<
                span<mutable_buffer const>>::value);
            char c[10] = "123456789";
            mutable_buffer b[3] = {
                mutable_buffer(c+0, 3),
                mutable_buffer(c+3, 3),
                mutable_buffer(c+6, 3)
            };
            span<mutable_buffer const> bs(&b[0], 3);
            test::check_sequence(bs, "123456789");
        }
    #if HAVE_SPAN
        {
            BOOST_STATIC_ASSERT(is_const_buffer_sequence<
                std::span<const_buffer const>>::value);
            const_buffer b[3] = {
                const_buffer("123", 3),
                const_buffer("456", 3),
                const_buffer("789", 3)
            };
            std::span<const_buffer const> bs(&b[0], 3);
            test::check_sequence(bs, "123456789");
        }
    #endif
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
