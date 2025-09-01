//
// Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/buffers
//

// Test that header file is self-contained.
#include <boost/buffers/mutable_buffer_span.hpp>

#include <boost/buffers/mutable_buffer_pair.hpp>
#include "test_buffers.hpp"

namespace boost {
namespace buffers {

struct mutable_buffer_span_test
{
    void
    testMembers()
    {
        std::string pat = test_pattern();
        mutable_buffer mb[3] = {
            { &pat[0], 3 },
            { &pat[3], 5 },
            { &pat[8], 7 } };

        // mutable_buffer_span()
        {
            mutable_buffer_span bs;
            BOOST_TEST_EQ(size(bs), 0);
        }

        // mutable_buffer_span(
        //  mutable_buffer const*,
        //  std::size_t n)
        {
            mutable_buffer_span mbs(mb, 3);
            test::check_sequence(mbs, pat);
        }

        // mutable_buffer_span(
        //  MutableBufferSequence)
        {
            mutable_buffer_pair bp;
            mutable_buffer_span sp(bp);
            BOOST_TEST(
                sp.end() - sp.begin() == 2);
        }

        // mutable_buffer_span(
        //  mutable_buffer_span const&)
        {
            mutable_buffer_span mbs0(mb, 3);
            mutable_buffer_span mbs1(mbs0);
            test::check_sequence(mbs1, pat);
        }

        // operator=(
        //  mutable_buffer_span const&)
        {
            mutable_buffer_span mbs0(mb, 3);
            mutable_buffer_span mbs1;
            mbs1 = mbs0;
            test::check_sequence(mbs1, pat);
        }
    }

    void
    run()
    {
        testMembers();
    }
};

TEST_SUITE(
    mutable_buffer_span_test,
    "boost.buffers.mutable_buffer_span");

} // buffers
} // boost
