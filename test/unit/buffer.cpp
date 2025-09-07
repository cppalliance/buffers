//
// Copyright (c) 2025 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/buffers
//

// Test that header file is self-contained.
#include <boost/buffers.hpp>

#include <boost/core/span.hpp>

#include "test_buffers.hpp"

/*
#if defined(__cpp_lib_span) && (__cpp_lib_span >= 202002L)
#define HAVE_STD_SPAN 1
#include <span>
#endif
*/

namespace boost {
namespace buffers {

BOOST_STATIC_ASSERT(  is_const_buffer_sequence<const_buffer>::value);
BOOST_STATIC_ASSERT(  is_const_buffer_sequence<mutable_buffer>::value);
BOOST_STATIC_ASSERT(! is_mutable_buffer_sequence<const_buffer>::value);
BOOST_STATIC_ASSERT(  is_mutable_buffer_sequence<mutable_buffer>::value);

BOOST_STATIC_ASSERT(  is_const_buffer_sequence<const_buffer const>::value);
BOOST_STATIC_ASSERT(  is_const_buffer_sequence<mutable_buffer const>::value);
BOOST_STATIC_ASSERT(! is_mutable_buffer_sequence<const_buffer const>::value);
BOOST_STATIC_ASSERT(  is_mutable_buffer_sequence<mutable_buffer const>::value);

BOOST_STATIC_ASSERT(  is_const_buffer_sequence<span<const_buffer>>::value);
BOOST_STATIC_ASSERT(  is_const_buffer_sequence<span<mutable_buffer>>::value);
BOOST_STATIC_ASSERT(! is_mutable_buffer_sequence<span<const_buffer>>::value);
BOOST_STATIC_ASSERT(  is_mutable_buffer_sequence<span<mutable_buffer>>::value);

BOOST_STATIC_ASSERT(  is_const_buffer_sequence<span<const_buffer const>>::value);
BOOST_STATIC_ASSERT(  is_const_buffer_sequence<span<mutable_buffer const>>::value);
BOOST_STATIC_ASSERT(! is_mutable_buffer_sequence<span<const_buffer const>>::value);
BOOST_STATIC_ASSERT(  is_mutable_buffer_sequence<span<mutable_buffer const>>::value);

struct buffer_test
{
    void
    testConstBuffer()
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

        // buffer sequence
        {
            auto const& pat = test_pattern();
            const_buffer cb(&pat[0], pat.size());
            test::check_sequence(cb, pat);
        }

        // boost::span
        {
            const_buffer b[3] = {
                const_buffer("123", 3),
                const_buffer("456", 3),
                const_buffer("789", 3)
            };
            span<const_buffer const> bs(&b[0], 3);
            test::check_sequence(bs, "123456789");
        }

        // std::span
        {
        #if HAVE_STD_SPAN
            BOOST_STATIC_ASSERT(is_const_buffer_sequence<
                std::span<const_buffer const>>::value);
            const_buffer b[3] = {
                const_buffer("123", 3),
                const_buffer("456", 3),
                const_buffer("789", 3)
            };
            std::span<const_buffer const> bs(&b[0], 3);
            test::check_sequence(bs, "123456789");
        #endif
        }
    }

    void
    testMutableBuffer()
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

        // buffer sequence
        {
            std::string pat = test_pattern();
            mutable_buffer cb(&pat[0], pat.size());
            test::check_sequence(cb, pat);
        }

        // boost::span
        {
            char c[10] = "123456789";
            mutable_buffer b[3] = {
                mutable_buffer(c+0, 3),
                mutable_buffer(c+3, 3),
                mutable_buffer(c+6, 3)
            };
            span<mutable_buffer const> bs(&b[0], 3);
            test::check_sequence(bs, "123456789");
        }

        // std::span
        {
        #if HAVE_STD_SPAN
            BOOST_STATIC_ASSERT(is_const_buffer_sequence<
                std::span<const_buffer const>>::value);
            const_buffer b[3] = {
                const_buffer("123", 3),
                const_buffer("456", 3),
                const_buffer("789", 3)
            };
            std::span<const_buffer const> bs(&b[0], 3);
            test::check_sequence(bs, "123456789");
        #endif
        }
    }

    void
    testSize()
    {
        char data[9];
        for(std::size_t i = 0; i < 3; ++i)
        for(std::size_t j = 0; j < 3; ++j)
        for(std::size_t k = 0; k < 3; ++k)
        {
            const_buffer cb[3] = {
                { data, i },
                { data + i, j },
                { data + i + j, k }
            };
            span<const_buffer const> s(cb, 3);
            BOOST_TEST_EQ(
                size(s), i + j + k);
        }
    }

    void
    run()
    {
        testConstBuffer();
        testMutableBuffer();
        testSize();
    }
};

TEST_SUITE(
    buffer_test,
    "boost.buffers.buffer");

} // buffers
} // boost
