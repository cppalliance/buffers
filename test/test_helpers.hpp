//
// Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/buffers
//

#ifndef BOOST_BUFFERS_TEST_HELPERS_HPP
#define BOOST_BUFFERS_TEST_HELPERS_HPP

#include <boost/buffers/range.hpp>
#include <string>
#include "test_suite.hpp"

namespace boost {
namespace buffers {

std::string const&
test_pattern()
{
    static std::string const pat =
        "012" "34567" "89abcde";
    return pat;
}

template<class T>
void
test_buffer_sequence(T& t)
{
    auto const& pat = test_pattern();
    auto const& ct = t;

    {
        std::string s;
        auto it = begin(t);
        auto const end_ = end(t);
        while(it != end_)
        {
            auto b = *it;
            s.append(static_cast<
                char const*>(b.data()),
                b.size());
            ++it;
        }
        BOOST_TEST_EQ(s, pat);
    }
    {
        std::string s;
        auto it = begin(t);
        auto const end_ = end(t);
        while(it != end_)
        {
            auto b = *it;
            s.append(static_cast<
                char const*>(b.data()),
                b.size());
            it++;
        }
        BOOST_TEST_EQ(s, pat);
    }
    {
        std::string s;
        auto it = begin(ct);
        auto const end_ = end(ct);
        while(it != end_)
        {
            auto b = *it;
            s.append(static_cast<
                char const*>(b.data()),
                b.size());
            ++it;
        }
        BOOST_TEST_EQ(s, pat);
    }
    {
        std::string s;
        auto it = begin(ct);
        auto const end_ = end(ct);
        while(it != end_)
        {
            auto b = *it;
            s.append(static_cast<
                char const*>(b.data()),
                b.size());
            it++;
        }
        BOOST_TEST_EQ(s, pat);
    }
    {
        std::string s;
        auto it = end(t);
        auto const begin_ = begin(t);
        while(it != begin_)
        {
            --it;
            auto b = *it;
            s.insert(0, static_cast<
                char const*>(b.data()),
                b.size());
        }
        BOOST_TEST_EQ(s, pat);
    }
    {
        std::string s;
        auto it = end(t);
        auto const begin_ = begin(t);
        while(it != begin_)
        {
            it--;
            auto b = *it;
            s.insert(0, static_cast<
                char const*>(b.data()),
                b.size());
        }
        BOOST_TEST_EQ(s, pat);
    }
    {
        std::string s;
        auto it = end(ct);
        auto const begin_ = begin(ct);
        while(it != begin_)
        {
            --it;
            auto b = *it;
            s.insert(0, static_cast<
                char const*>(b.data()),
                b.size());
        }
        BOOST_TEST_EQ(s, pat);
    }
    {
        std::string s;
        auto it = end(ct);
        auto const begin_ = begin(ct);
        while(it != begin_)
        {
            it--;
            auto b = *it;
            s.insert(0, static_cast<
                char const*>(b.data()),
                b.size());
        }
        BOOST_TEST_EQ(s, pat);
    }
}

} // buffers
} // boost

#endif
