//
// Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/buffers
//

#ifndef BOOST_BUFFERS_RANGE_HPP
#define BOOST_BUFFERS_RANGE_HPP

#include <boost/buffers/detail/config.hpp>
#include <boost/buffers/const_buffer.hpp>
#include <boost/buffers/mutable_buffer.hpp>
#include <boost/buffers/type_traits.hpp>
#include <type_traits>

namespace boost {
namespace buffers {

/** Return an iterator to the beginning of the buffer sequence.
*/
constexpr struct
{
    // mutable_buffer is a sequence of length 1
    template<class T>
    auto operator()(T const& t) const noexcept -> typename std::enable_if<
        std::is_convertible<T const*, mutable_buffer const*>::value,
        mutable_buffer const*>::type
    {
        return static_cast<mutable_buffer const*>(std::addressof(t));
    }

    // const_buffer is a sequence of length 1
    template<class T>
    auto operator()(T const& t) const noexcept -> typename std::enable_if<
        std::is_convertible<T const*, const_buffer const*>::value,
        const_buffer const*>::type
    {
        return static_cast<const_buffer const*>(std::addressof(t));
    }

    // Convertible T is a sequence of length 1
    template<class T>
    auto operator()(T const& t) const noexcept ->
        typename std::enable_if<
            ! std::is_convertible<T const*, const_buffer const*>::value &&
            ! std::is_convertible<T const*, mutable_buffer const*>::value && (
                std::is_convertible<T, const_buffer>::value ||
                std::is_convertible<T, mutable_buffer>::value)
        >::type
    {
        return std::addressof(t);
    }

    // Bidirectional range with convertible value type
    template<class T>
    auto operator()(T& t) const noexcept -> typename std::enable_if<
        ! std::is_convertible<T const*, const_buffer const*>::value &&
        ! std::is_convertible<T const*, mutable_buffer const*>::value &&
        ! std::is_convertible<T, const_buffer>::value &&
        ! std::is_convertible<T, mutable_buffer>::value &&
        detail::is_bidirectional_iterator<decltype(t.begin())>::value,
        decltype(t.begin())>::type
    {
        return t.begin();
    }

    // Bidirectional range with convertible value type
    template<class T>
    auto operator()(T const& t) const noexcept -> typename std::enable_if<
        ! std::is_convertible<T const*, const_buffer const*>::value &&
        ! std::is_convertible<T const*, mutable_buffer const*>::value &&
        ! std::is_convertible<T, const_buffer>::value &&
        ! std::is_convertible<T, mutable_buffer>::value &&
        detail::is_bidirectional_iterator<decltype(t.begin())>::value,
        decltype(t.begin())>::type
    {
        return t.begin();
    }
} begin {};

//------------------------------------------------------------------------------

/** Return an iterator to the end of the buffer sequence.
*/
constexpr struct
{
    // mutable_buffer is a sequence of length 1
    template<class T>
    auto operator()(T const& t) const noexcept -> typename std::enable_if<
        std::is_convertible<T const*, mutable_buffer const*>::value,
        mutable_buffer const*>::type
    {
        return static_cast<mutable_buffer const*>(std::addressof(t)) + 1;
    }

    // const_buffer is a sequence of length 1
    template<class T>
    auto operator()(T const& t) const noexcept -> typename std::enable_if<
        std::is_convertible<T const*, const_buffer const*>::value,
        const_buffer const*>::type
    {
        return static_cast<const_buffer const*>(std::addressof(t)) + 1;
    }

    // Convertible T is a sequence of length 1
    template<class T>
    auto operator()(T const& t) const noexcept ->
        typename std::enable_if<
            ! std::is_convertible<T const*, const_buffer const*>::value &&
            ! std::is_convertible<T const*, mutable_buffer const*>::value && (
                std::is_convertible<T, const_buffer>::value ||
                std::is_convertible<T, mutable_buffer>::value)
        >::type
    {
        return std::addressof(t) + 1;
    }

    // Bidirectional range with convertible value type
    template<class T>
    auto operator()(T& t) const noexcept -> typename std::enable_if<
        ! std::is_convertible<T const*, const_buffer const*>::value &&
        ! std::is_convertible<T const*, mutable_buffer const*>::value &&
        ! std::is_convertible<T, const_buffer>::value &&
        ! std::is_convertible<T, mutable_buffer>::value,
        decltype(t.end())>::type
    {
        return t.end();
    }

    // Bidirectional range with convertible value type
    template<class T>
    auto operator()(T const& t) const noexcept -> typename std::enable_if<
        ! std::is_convertible<T const*, const_buffer const*>::value &&
        ! std::is_convertible<T const*, mutable_buffer const*>::value &&
        ! std::is_convertible<T, const_buffer>::value &&
        ! std::is_convertible<T, mutable_buffer>::value,
        decltype(t.end())>::type
    {
        return t.end();
    }
} end {};

//------------------------------------------------

namespace detail {

template<class T>
class iter_range
{
    using begin_type = decltype(
        buffers::begin(std::declval<T&>()));
    using end_type = decltype(
        buffers::end(std::declval<T&>()));

    begin_type begin_;
    end_type end_;

public:
    iter_range(T& t) noexcept
        : begin_(buffers::begin(t))
        , end_(buffers::end(t))
    {
    }

    begin_type
    begin() const noexcept
    {
        return begin_;
    }

    end_type
    end() const noexcept
    {
        return end_;
    }
};

struct range_impl
{
    template<class BufferSequence>
    auto
    operator()(
        BufferSequence&& bs) const noexcept ->
            iter_range<typename
                std::remove_reference<
                    BufferSequence>::type>
    {
        return { bs };
    }
};

} // detail

/** Return a range representing the buffer sequence.
*/
constexpr detail::range_impl range{};

} // buffers
} // boost

#endif
