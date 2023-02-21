//
// Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/buffers
//

// Test that header file is self-contained.
#include <boost/buffers/circular_buffer.hpp>

#include "test_helpers.hpp"

namespace boost {
namespace buffers {

struct circular_buffer_test
{
    void
    testMembers()
    {
        std::string pat = test_pattern();

        // circular_buffer()
        {
            circular_buffer cb;
            BOOST_TEST_EQ(cb.size(), 0);
        }

        // circular_buffer(mutable_buffer)
        {
            circular_buffer cb(
                buffer(&pat[0], pat.size()));
            BOOST_TEST_EQ(cb.size(), 0);
            BOOST_TEST_EQ(cb.capacity(), pat.size());
            BOOST_TEST_EQ(cb.max_size(), pat.size());
        }

        // circular_buffer(
        //  circular_buffer const&)
        {
            circular_buffer cb0(
                buffer(&pat[0], pat.size()));
            circular_buffer cb1(cb0);
            BOOST_TEST_EQ(cb1.size(), cb0.size());
            BOOST_TEST_EQ(cb1.capacity(), cb0.capacity());
            BOOST_TEST_EQ(cb1.max_size(), cb0.max_size());
        }

        // operator=(
        //  circular_buffer const&)
        {
            circular_buffer cb0(
                buffer(&pat[0], pat.size()));
            circular_buffer cb1;
            cb1 = cb0;
            BOOST_TEST_EQ(cb1.size(), cb0.size());
            BOOST_TEST_EQ(cb1.capacity(), cb0.capacity());
            BOOST_TEST_EQ(cb1.max_size(), cb0.max_size());
        }

        // prepare(std::size_t)
        {
            circular_buffer cb(buffer(
                &pat[0], pat.size()));
            BOOST_TEST_THROWS(
                cb.prepare(cb.capacity() + 1),
                std::length_error);
        }
    }

    void
    testBuffer()
    {
        auto const& pat = test_pattern();

        for(std::size_t i = 0;
            i <= pat.size(); ++i)
        for(std::size_t j = 0;
            j <=  pat.size(); ++j)
        for(std::size_t k = 0;
            k <= pat.size(); ++k)
        {
            std::string s(pat.size(), 0);
            circular_buffer bs(
                &s[0], s.size());
            if( j < pat.size() &&
                i > 0)
            {
                bs.prepare(i);
                bs.commit(i);
                bs.consume(i - 1);
                bs.commit(buffer_copy(
                    bs.prepare(j),
                    buffer(
                        pat.data(),
                        pat.size())));
                bs.consume(1);
            }
            else
            {
                bs.commit(buffer_copy(
                    bs.prepare(j),
                    buffer(
                        pat.data(),
                        pat.size())));
            }
            bs.commit(buffer_copy(
                bs.prepare(pat.size() - j),
                buffer(
                    pat.data() + j,
                    pat.size() - j)));
            BOOST_TEST_EQ(test_to_string(
                bs.data()), pat);
            test_buffer_sequence(bs.data());
            bs.consume(k);
            BOOST_TEST_EQ(test_to_string(
                bs.data()), pat.substr(k));
        }
    }

    void
    run()
    {
        testMembers();
        testBuffer();
    }
};

TEST_SUITE(
    circular_buffer_test,
    "boost.buffers.circular_buffer");

} // buffers
} // boost
