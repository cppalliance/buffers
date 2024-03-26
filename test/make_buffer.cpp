//
// Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/buffers
//

// Test that header file is self-contained.
#include <boost/buffers/make_buffer.hpp>

#include <boost/core/span.hpp>
#include <vector>
#include <memory>

#include "boost/buffers/type_traits.hpp"
#include "test_suite.hpp"

namespace boost {
namespace buffers {

struct make_buffer_test
{
    void
    testBuffer()
    {
        char buf[3]{};
        char const* cbuf = buf;

        // make_buffer(mutable_buffer)
        {
            mutable_buffer b(buf, 3);
            auto b1 = make_buffer(b);
            BOOST_TEST_EQ(b1.data(), b.data());
            BOOST_TEST_EQ(b1.size(), b.size());
        }

        // make_buffer(void*, std::size_t)
        {
            auto b = make_buffer(buf, 3);
            BOOST_TEST_EQ(b.data(), buf);
            BOOST_TEST_EQ(b.size(), 3);
        }

        // make_buffer(const_buffer)
        {
            const_buffer b(cbuf, 3);
            auto b1 = make_buffer(b);
            BOOST_TEST_EQ(b1.data(), b.data());
            BOOST_TEST_EQ(b1.size(), b.size());
        }

        // make_buffer(void const*, std::size_t)
        {
            auto b = make_buffer(cbuf, 3);
            BOOST_TEST_EQ(b.data(), cbuf);
            BOOST_TEST_EQ(b.size(), 3);
        }

        // make_buffer(T(&)[N])
        {
            mutable_buffer b = make_buffer(buf);
            BOOST_TEST_EQ(b.data(), buf);
            BOOST_TEST_EQ(b.size(), 3);
        }

        // make_buffer(T const(&)[N])
        {
            char const cbuf3[3]{};
            const_buffer b = make_buffer(cbuf3);
            BOOST_TEST_EQ(b.data(), cbuf3);
            BOOST_TEST_EQ(b.size(), 3);
        }

        // make_buffer(T&)
        {
            std::vector<int> buf{1, 2, 3, 4};
            mutable_buffer b = make_buffer(buf);
            BOOST_TEST_EQ(b.data(), buf.data());
            BOOST_TEST_EQ(b.size(), sizeof(int) * buf.size());

            boost::span<int> s(buf);
            mutable_buffer b2 = make_buffer(s);
            BOOST_TEST_EQ(b2.data(), buf.data());
            BOOST_TEST_EQ(b2.size(), sizeof(int) * buf.size());
        }
        {
            std::vector<int> const buf{1, 2, 3, 4};
            const_buffer b = make_buffer(buf);
            BOOST_TEST_EQ(b.data(), buf.data());
            BOOST_TEST_EQ(b.size(), sizeof(int) * buf.size());

            boost::span<int const> s(buf);
            const_buffer b2 = make_buffer(s);
            BOOST_TEST_EQ(b2.data(), buf.data());
            BOOST_TEST_EQ(b2.size(), sizeof(int) * buf.size());
        }
        {
            BOOST_TEST(
                detail::is_mutable_contiguous_container<
                    std::vector<int>&
                >::value);
            BOOST_TEST(
                !detail::is_const_contiguous_container<
                    std::vector<int>&
                >::value);
            BOOST_TEST(
                detail::is_const_contiguous_container<
                    std::vector<int> const&
                >::value);
            BOOST_TEST(
                !detail::is_mutable_contiguous_container<
                    std::vector<std::unique_ptr<int>>&
                >::value);
            BOOST_TEST(
                !detail::is_const_contiguous_container<
                    std::vector<std::unique_ptr<int>> const&
                >::value);
        }
    }

    void
    run()
    {
        testBuffer();
    }
};

TEST_SUITE(
    make_buffer_test,
    "boost.buffers.make_buffer");

} // buffers
} // boost
