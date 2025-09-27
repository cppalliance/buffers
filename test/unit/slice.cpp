//
// Copyright (c) 2025 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/buffers
//

// Test that header file is self-contained.
#include <boost/buffers/slice.hpp>

#include <boost/buffers/buffer_pair.hpp>
#include <boost/buffers/copy.hpp>
#include <boost/buffers/make_buffer.hpp>
#include <boost/core/detail/string_view.hpp>
#include <boost/static_assert.hpp>

#include <array>

#include "test_buffers.hpp"
#include "test_suite.hpp"

namespace boost {
namespace buffers {

template<
    std::size_t I,
    std::size_t N>
void
set(
    std::string&,
    std::array<const_buffer, N>&)
{
}

template<
    std::size_t I,
    std::size_t N,
    class... Args>
void
set(
    std::string& s,
    std::array<const_buffer, N>& v,
    char const* p,
    Args const&... args)
{
    core::string_view sv(p);
    v[I] = const_buffer(sv.data(), sv.size());
    s.append(sv.data(), sv.size());
    set<I+1>(s, v, args...);
}

auto
make_buffers(
    std::string&) ->
    std::array<const_buffer, 0>
{
    return {};
}

template<
    class... Args>
auto
make_buffers(
    std::string& s,
    char const* arg0,
    Args const&... args) ->
    std::array<const_buffer, 1 + sizeof...(Args)>
{
    s = {};
    std::array<const_buffer, 1 + sizeof...(Args)> v;
    set<0>(s, v, arg0, args...);
    return v;
}

struct slice_test
{
    static
    void
    checkStatic()
    {
        using T = slice_of<const_buffer_pair>;

        BOOST_STATIC_ASSERT(std::is_default_constructible<T>::value);
        BOOST_STATIC_ASSERT(std::is_copy_constructible<T>::value);
        BOOST_STATIC_ASSERT(std::is_move_constructible<T>::value);
        BOOST_STATIC_ASSERT(std::is_copy_assignable<T>::value);
        BOOST_STATIC_ASSERT(std::is_move_assignable<T>::value);

        using U = T::const_iterator;

        BOOST_STATIC_ASSERT(std::is_default_constructible<U>::value);
        BOOST_STATIC_ASSERT(std::is_copy_constructible<U>::value);
        BOOST_STATIC_ASSERT(std::is_move_constructible<U>::value);
        BOOST_STATIC_ASSERT(std::is_copy_assignable<U>::value);
        BOOST_STATIC_ASSERT(std::is_move_assignable<U>::value);
    }

    template<class B>
    static
    void
    check(
        B const& b,
        core::string_view s)
    {
        auto constexpr M = 1024;
        char buf[M];
        if(! BOOST_TEST_LE(size(b), M))
            return;
        if(! BOOST_TEST_EQ(size(b), s.size()))
            return;
        auto const n = copy(
            mutable_buffer(buf, M), b);
        if(! BOOST_TEST_EQ(n, s.size()))
            return;
        if(! BOOST_TEST_EQ(core::string_view(buf, n), s))
            return;
        test::check_iterators(b, s);
    }

    using seq_type = std::array<const_buffer, 3>;

    void
    grind_back(
        slice_of<seq_type> const& bs0,
        core::string_view pat0)
    {
        auto const n = size(bs0);
        if(! BOOST_TEST_EQ(n, pat0.size()))
            return;
        for(std::size_t i = 0; i < n; ++i)
        {
            auto bs = bs0;
            auto pat = pat0.substr(0, pat0.size() - i);
            remove_suffix(bs, i);
            check(bs, pat);
        }
        // n >= size
        for(std::size_t i = 0; i < 2; ++i)
        {
            auto bs = bs0;
            remove_suffix(bs, n + i);
            BOOST_TEST_EQ(size(bs), 0);
            check(bs, "");
        }
    }

    void
    grind(
        slice_of<seq_type> const& bs0,
        core::string_view pat0)
    {
        auto const n = size(bs0);
        if(! BOOST_TEST_EQ(n, pat0.size()))
            return;
        for(std::size_t i = 0; i < n; ++i)
        {
            auto bs = bs0;
            auto pat = pat0.substr(i);
            remove_prefix(bs, i);
            check(bs, pat);
            grind_back(bs, pat);
        }
        // n >= size
        for(std::size_t i = 0; i < 2; ++i)
        {
            auto bs = bs0;
            remove_prefix(bs, n + i);
            BOOST_TEST_EQ(size(bs), 0);
            check(bs, "");
        }
    }

    void
    run()
    {
        std::string s;
        seq_type bs =  make_buffers(s,
            "boost.", "buffers.", "slice_" );
        test::check_sequence(bs, s);
        //check(bs, s);
        //grind(bs, s);
    }
};

TEST_SUITE(
    slice_test,
    "boost.buffers.slice");

} // buffers
} // boost
