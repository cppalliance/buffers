//
// Copyright (c) 2025 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/buffers
//

// Test that header file is self-contained.
#include <boost/buffers/any.hpp>

#include <boost/buffers/buffer_pair.hpp>
#include <boost/static_assert.hpp>

#include "test_buffers.hpp"

namespace boost {
namespace buffers {

using It = any<true>::iterator;

BOOST_STATIC_ASSERT(std::is_same<
    decltype(std::declval<any<true> const>().begin()), It>::value);
BOOST_STATIC_ASSERT(std::is_same<
    decltype(std::declval<any<true> const>().end()), It>::value);
BOOST_STATIC_ASSERT(
       std::is_destructible<It>::value
    && std::is_default_constructible<It>::value
/*    && std::is_copy_constructible<It>::value
    && std::is_copy_assignable<It>::value
    && std::is_move_constructible<It>::value
    && std::is_move_assignable<It>::value*/
);

BOOST_STATIC_ASSERT(
    detail::is_bidirectional_iterator<It>::value);

//BOOST_STATIC_ASSERT(std::is_convertible<
//    decltype(*std::declval<any<true>::iterator const&>()),
//    const_buffer>::value);

//BOOST_STATIC_ASSSERT(detail::is_bidirectional_iterator<
    //decltype(*std::declval<any<true>::iterator const&>()));;

struct any_test
{
    static const_buffer_pair make_pair(
        core::string_view s0,
        core::string_view s1)
    {
        return {
            { s0.data(), s0.size() },
            { s1.data(), s1.size() } };
    }

    void
    test_param(any_const_ref cb)
    {
        core::string_view pat = test_pattern();
        test::check_sequence(cb, pat);
    }

    void
    run()
    {
        core::string_view pat = test_pattern();
        auto cb = make_pair(pat.substr(0, 3), pat.substr(3));
        test::check_sequence(cb, pat);
        test::check_sequence(make_any(cb), pat);

        test_param(make_any(cb));
    }
};

TEST_SUITE(
    any_test,
    "boost.buffers.any");

} // buffers
} // boost
