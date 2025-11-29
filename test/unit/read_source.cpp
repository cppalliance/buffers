//
// Copyright (c) 2025 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/buffers
//

// Test that header file is self-contained.
#include <boost/buffers/read_source.hpp>

#include <boost/buffers/error.hpp>
#include <boost/buffers/slice.hpp>

#include <boost/core/detail/static_assert.hpp>
#include <boost/core/detail/string_view.hpp>
#include <boost/core/span.hpp>

#include <cstring>

#include "test_suite.hpp"

namespace boost {
namespace buffers {

namespace {

struct test_source
{
    core::string_view s_;
    std::size_t nread_ = 0;

    explicit test_source(
        core::string_view s)
        : s_(s)
    {
    }

    template<class MutableBufferSequence>
    std::size_t read(
        MutableBufferSequence const& dest,
        system::error_code& ec)
    {
        if(nread_ >= s_.size())
        {
            ec = error::eof;
            return 0;
        }
        auto const n = copy(dest, sans_prefix(
            const_buffer(s_.data(), s_.size()), nread_));
        nread_ += n;
        if(nread_ >= s_.size())
            ec = error::eof;
        return n;
    }
};

// has_sizee
struct test_source2 : test_source
{
    explicit test_source2(
        core::string_view s)
        : test_source(s)
    {
    }

    std::uint64_t
    size() const noexcept
    {
        return s_.size();
    }
};

// has_rewind
struct test_source3 : test_source
{
    explicit test_source3(
        core::string_view s)
        : test_source(s)
    {
    }

    void rewind()
    {
        nread_ = 0;
    }
};

// has_size, has_rewind
struct test_source4 : test_source3
{
    explicit test_source4(
        core::string_view s)
        : test_source3(s)
    {
    }

    std::uint64_t
    size() const noexcept
    {
        return s_.size();
    }
};

struct not_source1
{
};

struct not_source2
{
    void read(system::error_code&)
    {
    }
};

struct data_source
{
    const_buffer data() const noexcept
    {
        return {};
    }
};

} // (anon)

BOOST_CORE_STATIC_ASSERT(  is_read_source<test_source>::value);
BOOST_CORE_STATIC_ASSERT(! is_read_source<int>::value);
BOOST_CORE_STATIC_ASSERT(! is_read_source<not_source1>::value);
BOOST_CORE_STATIC_ASSERT(! is_read_source<not_source2>::value);
BOOST_CORE_STATIC_ASSERT(! is_read_source<data_source>::value);

BOOST_CORE_STATIC_ASSERT(! has_size<test_source>::value);
BOOST_CORE_STATIC_ASSERT(  has_size<test_source2>::value);
BOOST_CORE_STATIC_ASSERT(! has_size<test_source3>::value);
BOOST_CORE_STATIC_ASSERT(  has_size<test_source4>::value);

BOOST_CORE_STATIC_ASSERT(! has_rewind<test_source>::value);
BOOST_CORE_STATIC_ASSERT(! has_rewind<test_source2>::value);
BOOST_CORE_STATIC_ASSERT(  has_rewind<test_source3>::value);
BOOST_CORE_STATIC_ASSERT(  has_rewind<test_source4>::value);

struct read_source_test
{
    void
    testReadFromSingleBuffer()
    {
        // Test basic functionality
        {
            core::string_view s = "Hello, World!";
            std::size_t pos = 0;

            auto read_fn = [&](mutable_buffer mb, system::error_code& ec) -> std::size_t
            {
                if(pos >= s.size())
                {
                    ec = error::eof;
                    return 0;
                }
                auto const n = (std::min)(mb.size(), s.size() - pos);
                std::memcpy(mb.data(), s.data() + pos, n);
                pos += n;
                if(pos >= s.size())
                    ec = error::eof;
                return n;
            };

            char buf[20];
            mutable_buffer mb(buf, sizeof(buf));
            system::error_code ec;
            auto const nread = read_from_single_buffer(mb, ec, read_fn);

            BOOST_TEST(nread == s.size());
            BOOST_TEST(! ec.failed());
            BOOST_TEST(core::string_view(buf, nread) == s);
        }

        // Test EOF on first call
        {
            auto read_fn = [](mutable_buffer, system::error_code& ec) -> std::size_t
            {
                ec = error::eof;
                return 0;
            };

            char buf[10];
            mutable_buffer mb(buf, sizeof(buf));
            system::error_code ec;
            auto const nread = read_from_single_buffer(mb, ec, read_fn);

            BOOST_TEST(nread == 0);
            BOOST_TEST(ec == error::eof);
        }

        // Test EOF after partial read
        {
            core::string_view s = "Hi";
            std::size_t call_count = 0;

            auto read_fn = [&](mutable_buffer mb, system::error_code& ec) -> std::size_t
            {
                ++call_count;
                if(call_count == 1)
                {
                    // First call: read 2 bytes
                    auto const n = (std::min)(mb.size(), s.size());
                    std::memcpy(mb.data(), s.data(), n);
                    return n;
                }
                // Second call: EOF
                ec = error::eof;
                return 0;
            };

            char buf1[5];
            char buf2[5];
            mutable_buffer bufs[2] = { mutable_buffer(buf1, 5), mutable_buffer(buf2, 5) };
            system::error_code ec;
            auto const nread = read_from_single_buffer(
                span<mutable_buffer const>(bufs, 2), ec, read_fn);

            BOOST_TEST(nread == 2);
            BOOST_TEST(! ec.failed());
        }

        // Test empty buffers
        {
            auto read_fn = [](mutable_buffer mb, system::error_code&) -> std::size_t
            {
                return mb.size();
            };

            mutable_buffer empty_buf(nullptr, 0);
            system::error_code ec;
            auto const nread = read_from_single_buffer(empty_buf, ec, read_fn);

            BOOST_TEST(nread == 0);
            BOOST_TEST(! ec.failed());
        }

        // Test error propagation
        {
            auto read_fn = [](mutable_buffer, system::error_code& ec) -> std::size_t
            {
                ec = system::error_code(22, system::generic_category());
                return 0;
            };

            char buf[10];
            mutable_buffer mb(buf, sizeof(buf));
            system::error_code ec;
            auto const nread = read_from_single_buffer(mb, ec, read_fn);

            BOOST_TEST(nread == 0);
            BOOST_TEST(ec.failed());
            BOOST_TEST(ec.value() == 22);
        }
    }

    void run()
    {
        testReadFromSingleBuffer();
    }
};

TEST_SUITE(
    read_source_test,
    "boost.buffers.read_source");

} // buffers
} // boost

