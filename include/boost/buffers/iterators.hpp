//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/http_proto
//

#ifndef BOOST_BUFFERS_RANGE_HPP
#define BOOST_BUFFERS_RANGE_HPP

#include <boost/buffers/detail/config.hpp>
#include <boost/buffers/const_buffer.hpp>
#include <boost/buffers/mutable_buffer.hpp>
#include <boost/buffers/type_traits.hpp>

namespace boost {
namespace buffers {

#ifdef BOOST_BUFFERS_DOCS

/** Return an iterator to the beginning of the buffer sequence.
*/
template<class BufferSequence>
__see_below__
begin(BufferSequence&& b) noexcept ->

/** Return an iterator to the end of the buffer sequence.
*/
template<class BufferSequence>
__see_below__
end(BufferSequence&& b) noexcept ->

/** Return a range representing the buffer sequence.
*/
template<class BufferSequence>
__see_below__
range(BufferSequence&& bs) noexcept;

#else

template<class MutableBuffer>
auto
begin(MutableBuffer const& b) noexcept ->
    typename std::enable_if<
    std::is_convertible<
        MutableBuffer const*,
        mutable_buffer const*>::value,
    mutable_buffer const*>::type
{
    return static_cast<
        mutable_buffer const*>(
            std::addressof(b));
}

template<class ConstBuffer>
auto
begin(ConstBuffer const& b) noexcept ->
    typename std::enable_if<
    std::is_convertible<
        ConstBuffer const*,
        const_buffer const*>::value,
    const_buffer const*>::type
{
    return static_cast<
        const_buffer const*>(
            std::addressof(b));
}

template<class BufferSequence>
auto
begin(BufferSequence& bs) noexcept ->
    typename std::enable_if<
    ! std::is_convertible<
        BufferSequence const*,
        const_buffer const*>::value &&
    ! std::is_convertible<
        BufferSequence const*,
        mutable_buffer const*>::value,
    decltype(bs.begin())>::type
{
    return bs.begin();
}

template<class BufferSequence>
auto
begin(BufferSequence const& bs) noexcept ->
    typename std::enable_if<
    ! std::is_convertible<
        BufferSequence const*,
        const_buffer const*>::value &&
    ! std::is_convertible<
        BufferSequence const*,
        mutable_buffer const*>::value,
    decltype(bs.begin())>::type
{
    return bs.begin();
}

template<class MutableBuffer>
auto
end(MutableBuffer const& b) noexcept ->
    typename std::enable_if<
    std::is_convertible<
        MutableBuffer const*,
        mutable_buffer const*>::value,
    mutable_buffer const*>::type
{
    return static_cast<
        mutable_buffer const*>(
            std::addressof(b)) + 1;
}

template<class ConstBuffer>
auto
end(ConstBuffer const& b) noexcept ->
    typename std::enable_if<
    std::is_convertible<
        ConstBuffer const*,
        const_buffer const*>::value,
    const_buffer const*>::type
{
    return static_cast<
        const_buffer const*>(
            std::addressof(b)) + 1;
}

template<class BufferSequence>
auto
end(BufferSequence& bs) noexcept ->
    typename std::enable_if<
    ! std::is_convertible<
        BufferSequence const*,
        const_buffer const*>::value &&
    ! std::is_convertible<
        BufferSequence const*,
        mutable_buffer const*>::value,
    decltype(bs.end())>::type
{
    return bs.end();
}

template<class BufferSequence>
auto
end(BufferSequence const& bs) noexcept ->
    typename std::enable_if<
    ! std::is_convertible<
        BufferSequence const*,
        const_buffer const*>::value &&
    ! std::is_convertible<
        BufferSequence const*,
        mutable_buffer const*>::value,
    decltype(bs.end())>::type
{
    return bs.end();
}

namespace detail {

template<class T>
struct range_type
{
    using begin_t = decltype(
        buffers::begin(std::declval<T&>()));
    using end_t = decltype(
        buffers::end(std::declval<T&>()));

    begin_t begin_;
    end_t end_;

    explicit
    range_type(T& t) noexcept
        : begin_(buffers::begin(t))
        , end_(buffers::end(t))
    {
    }

    begin_t
    begin() const noexcept
    {
        return begin_;
    }

    end_t
    end() const noexcept
    {
        return end_;
    }
};

} // detail

template<class BufferSequence>
auto
range(BufferSequence& bs) noexcept ->
    detail::range_type<BufferSequence>
{
    return { bs };
}

template<class BufferSequence>
auto
range(BufferSequence const& bs) noexcept ->
    detail::range_type<BufferSequence const>
{
    return { bs };
}

#endif

} // buffers
} // boost

#endif
