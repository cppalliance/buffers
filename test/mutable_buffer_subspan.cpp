//
// Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/buffers
//

// Test that header file is self-contained.
#include <boost/buffers/mutable_buffer_subspan.hpp>

#include <boost/buffers/mutable_buffer_span.hpp>
#include <boost/buffers/sans_prefix.hpp>
#include <boost/buffers/sans_suffix.hpp>
#include "test_helpers.hpp"

namespace boost {
namespace buffers {

struct mutable_buffer_subspan_test
{
    void
    testMembers()
    {
        std::string pat = test_pattern();
        mutable_buffer const mb[3] = {
            { &pat[0], 3 },
            { &pat[0] + 3, 5 },
            { &pat[0] + 8, 7 } };

        // mutable_buffer_subspan()
        {
            mutable_buffer_subspan bs;
            BOOST_TEST_EQ(size(bs), 0);
        }

        // mutable_buffer_subspan(
        //  mutable_buffer const*, std::size_t)
        {
            mutable_buffer_subspan s(mb, 3);
            BOOST_TEST_EQ(size(s), 15);
        }
        {
            mutable_buffer_subspan s(mb, 0);
            BOOST_TEST_EQ(size(s), 0);
        }

        // mutable_buffer_subspan(
        //  mutable_buffer_span const&)
        {
            mutable_buffer_span ms0(mb, 3);
            mutable_buffer_subspan ms1(ms0);
            BOOST_TEST_EQ(
                size(ms0),
                size(ms1));
        }
        {
            mutable_buffer_span ms0(mb, 0);
            mutable_buffer_subspan ms1(ms0);
            BOOST_TEST_EQ(
                size(ms0),
                size(ms1));
        }

        // mutable_buffer_subspan(
        //  mutable_buffer_subspan const&)
        {
            mutable_buffer_subspan s0(mb, 3);
            mutable_buffer_subspan s1(s0);
            BOOST_TEST_EQ(
                size(s1),
                size(s0));
        }

        // operator=(
        //  mutable_buffer_subspan const&)
        {
            mutable_buffer_subspan s;
            BOOST_TEST_EQ(size(s), 0);
            s = mutable_buffer_subspan(mb, 3);
            BOOST_TEST_EQ(size(s), 15);
        }
    }

    void
    testSequence()
    {
        std::string pat = test_pattern();

        // length 1
        {
            mutable_buffer mb = {
                &pat[0], pat.size() };
            mutable_buffer_subspan s(&mb, 1);
            test_buffer_sequence(s);
        }

        // length 2
        {
            mutable_buffer const mb[2] = {
                { &pat[0], 7 },
                { &pat[7], 8 } };
            mutable_buffer_subspan s(mb, 2);
            test_buffer_sequence(s);
        }

        // length 3
        {
            mutable_buffer const mb[3] = {
                { &pat[0], 3 },
                { &pat[3], 5 },
                { &pat[8], 7 } };
            mutable_buffer_subspan s(mb, 3);
            test_buffer_sequence(s);
        }
    }

    void
    testSubspan()
    {
        std::string tmp;
        std::string pat = test_pattern();
        mutable_buffer const mb[3] = {
            { &pat[0], 3 },
            { &pat[3], 5 },
            { &pat[8], 7 } };
        mutable_buffer_span const cs0(mb, 3);

        for(std::size_t i = 0; i <= pat.size(); ++i )
        {
            {
                auto b0 = prefix(cs0, i);
                auto b1 = sans_prefix(cs0, i);
                tmp = std::string(pat.size(), ' ');
                mutable_buffer dest(&tmp[0], tmp.size());
                auto n = copy(dest, b0);
                dest = sans_prefix(dest, n);
                n += copy(dest, b1);
                BOOST_TEST_EQ(n, pat.size());
                BOOST_TEST_EQ(tmp, pat);
            }
            for(std::size_t j = 0; j <= pat.size(); ++j)
            {
                auto b = prefix(sans_prefix(cs0, i), j);
                tmp.resize(pat.size());
                tmp.resize(copy(
                    mutable_buffer(
                        &tmp[0], tmp.size()), b));
                if(i <= pat.size())
                    BOOST_TEST_EQ(tmp, pat.substr(i, j));
                else
                    BOOST_TEST(tmp.empty());
            }
            for(std::size_t j = 0; j <= pat.size(); ++j)
            {
                auto b = suffix(sans_suffix(cs0, i), j);
                tmp.resize(pat.size());
                tmp.resize(copy(
                    mutable_buffer(
                        &tmp[0], tmp.size()), b));
                if(i <= pat.size())
                {
                    auto n = pat.size() - i; // inner length
                    if(n >= j)
                        BOOST_TEST_EQ(tmp,
                            pat.substr(n - j, j));
                    else
                        BOOST_TEST_EQ(tmp,
                            pat.substr(0, n));
                }
                else
                {
                    BOOST_TEST(tmp.empty());
                }
            }
        }
    }

    void
    run()
    {
        testMembers();
        testSequence();
        testSubspan();
    }
};

TEST_SUITE(
    mutable_buffer_subspan_test,
    "boost.buffers.mutable_buffer_subspan");

} // buffers
} // boost
