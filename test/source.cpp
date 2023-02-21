//
// Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/buffers
//

// Test that header file is self-contained.
#include <boost/buffers/source.hpp>

#include "test_helpers.hpp"

namespace boost {
namespace buffers {

struct source_test
{
    static
    const_buffer
    pattern()
    {
        return const_buffer(
            "0123456789"
            "0123456789"
            "0123456789"
            "0123456789", 40);
    }

    template<class Bs0, class Bs1>
    static
    bool
    equal(
        Bs0 const& bs0,
        Bs1 const& bs1)
    {
        auto const n =
            buffer_size(bs0);
        if(buffer_size(bs1) != n)
            return false;
        auto p = new char[n * 2];
        buffer_copy(
            mutable_buffer(p, n), bs0);
        buffer_copy(
            mutable_buffer(p + n, n), bs1);
        auto const result =
            std::memcmp(p, p + n, n) == 0;
        delete[] p;
        return result;
    }

    struct test_source : source
    {
        test_source(
            std::size_t max_size,
            std::size_t alloc,
            std::size_t n_success =
                std::size_t(-1)) noexcept
            : max_size_(max_size)
            , alloc_(alloc)
            , n_success_(n_success)
            , cb_(pattern())
        {
        }

        void
        init(allocator& a) override
        {
            BOOST_TEST(
                a.max_size() == max_size_);
            a.allocate(alloc_);
        }

        results
        on_read(
            mutable_buffer b) override
        {
            results rv;
            if(n_success_-- == 0)
            {
                rv.ec = boost::system::error_code(
                    boost::system::errc::invalid_argument,
                    boost::system::generic_category());
                return rv;
            }
            auto const n = buffer_copy(b, cb_);
            cb_ += n;
            rv.bytes += n;
            rv.finished = cb_.size() == 0;
            return rv;
        }

    private:
        std::size_t max_size_;
        std::size_t alloc_;
        std::size_t n_success_;
        const_buffer cb_;
    };

    void
    testSource()
    {
        // max_size == 0
        {
            buffered_base::allocator a;
            test_source ts(0, 0);
            ts.init(a);
        }

        // max_size > 0
        {
            {
                char tmp[10];
                buffered_base::allocator a(
                    tmp, sizeof(tmp), false);
                test_source ts(10, 0);
                ts.init(a);
            }
            {
                char tmp[10];
                buffered_base::allocator a(
                    tmp, sizeof(tmp), false);
                test_source ts(10, 6);
                ts.init(a);
            }
        }

        // max_size exceeded
        {
            char tmp[10];
            buffered_base::allocator a(
                tmp, sizeof(tmp), false);
            test_source ts(10, 20);
            BOOST_TEST_THROWS(
                ts.init(a),
                std::invalid_argument);
        }

        // 40 character string
        {
            char tmp[40];
            buffered_base::allocator a;
            test_source ts(0, 0);
            ts.init(a);
            ts.read(buffer(tmp));
            BOOST_TEST(equal(
                const_buffer(tmp, sizeof(tmp)),
                pattern()));
        }
    }

    void
    run()
    {
        testSource();
    }
};

TEST_SUITE(
    source_test,
    "boost.buffers.source");

} // buffers
} // boost
