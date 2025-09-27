//
// Copyright (c) 2025 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/buffers
//

#ifndef BOOST_BUFFERS_SLICE_HPP
#define BOOST_BUFFERS_SLICE_HPP

#include <boost/buffers/detail/config.hpp>
#include <boost/buffers/buffer.hpp>
#include <boost/buffers/range.hpp>
#include <boost/assert.hpp>
#include <array>
#include <iterator>
#include <type_traits>

namespace boost {
namespace buffers {

template<class T> class slice_of;

namespace detail {

template<class T, class = void>
struct has_tag_invoke : std::false_type {};

template<class T>
struct has_tag_invoke<T, decltype(tag_invoke(
    std::declval<slice_tag const&>(),
    std::declval<T&>(),
    std::declval<slice_how>(),
    std::declval<std::size_t>()))>
    : std::true_type {};

} // detail

/** Alias for the type representing a slice of T
*/
template<class T>
using slice_type = typename std::conditional<
    detail::has_tag_invoke<T>::value,
    T, slice_of<T> >::type;

//------------------------------------------------

/** A wrapper enabling a buffer sequence to be consumed
*/
template<class BufferSequence>
class slice_of
{
    static_assert(! std::is_const<BufferSequence>::value,
        "BufferSequence can't be const");

    static_assert(! std::is_reference<BufferSequence>::value,
        "BufferSequence can't be a reference");

    static_assert(is_const_buffer_sequence<BufferSequence>::value,
        "BufferSequence does not meet type requirements");

    using iter_type = decltype(
        std::declval<BufferSequence const&>().begin());

    BufferSequence bs_;
    iter_type begin_;
    iter_type end_;
    std::size_t len_ = 0;       // length of bs_
    std::size_t size_ = 0;      // total bytes
    std::size_t prefix_ = 0;    // used prefix bytes
    std::size_t suffix_ = 0;    // used suffix bytes

public:
    /** The type of values returned by iterators
    */
    using value_type = typename std::conditional<
        is_mutable_buffer_sequence<BufferSequence>::value,
        mutable_buffer, const_buffer>::type;

    /** The type of returned iterators
    */
    class const_iterator;

    /** Constructor
    */
    slice_of() = default;

    /** Constructor
    */
    slice_of(
        BufferSequence const& bs)
        : bs_(bs)
        , begin_(buffers::begin(bs_))
        , end_(buffers::end(bs_))
    {
        auto it = begin_;
        while(it != end_)
        {
            value_type b(*it);
            size_ += b.size();
            ++len_;
            ++it;
        }
    }

    /** Return an iterator to the beginning of the sequence
    */
    const_iterator
    begin() const noexcept;

    /** Return an iterator to the end of the sequence
    */
    const_iterator
    end() const noexcept;

    friend
    void
    tag_invoke(
        slice_tag const&,
        slice_of<BufferSequence>& bs,
        slice_how how,
        std::size_t n)
    {
        bs.slice_impl(how, n);
    }

private:
    void
    remove_prefix_impl(
        std::size_t n)
    {
        // nice hack to simplify the loop (M. Nejati)
        n += prefix_;
        size_ += prefix_;
        prefix_ = 0;

        while(n > 0 && begin_ != end_)
        {
            value_type b = *begin_;
            if(n < b.size())
            {
                prefix_ = n;
                size_ -= n;
                break;
            }
            n -= b.size();
            size_ -= b.size();
            ++begin_;
            --len_;
        }
    }

    void
    remove_suffix_impl(
        std::size_t n)
    {
        if(size_ == 0)
        {
            BOOST_ASSERT(begin_ == end_);
            return;
        }
        BOOST_ASSERT(begin_ != end_);
        n += suffix_;
        size_ += suffix_;
        suffix_ = 0;
        iter_type it = end_;
        --it;
        while(it != begin_)
        {
            value_type b = *it;
            if(n < b.size())
            {
                suffix_ = n;
                size_ -= n;
                return;
            }
            n -= b.size();
            size_ -= b.size();
            --it;
            --end_;
            --len_;
        }
        value_type b = *it;
        auto m = b.size() - prefix_;
        if(n < m)
        {
            suffix_ = n;
            size_ -= n;
            return;
        }
        end_ = begin_;
        len_ = 0;
    }

    void
    keep_prefix_impl(
        std::size_t n)
    {
        if(n >= size_)
            return;
        if(n == 0)
        {
            end_ = begin_;
            len_ = 0;
            size_ = 0;
            return;
        }
        remove_suffix_impl(size_ - n);
    }

    void
    keep_suffix_impl(
        std::size_t n)
    {
        if(n >= size_)
            return;
        if(n == 0)
        {
            begin_ = end_;
            len_ = 0;
            size_ = 0;
            return;
        }
        remove_prefix_impl(size_ - n);
    }

    void
    slice_impl(
        slice_how how,
        std::size_t n)
    {
        switch(how)
        {
        case slice_how::remove_prefix:
        {
            remove_prefix_impl(n);
            break;
        }
        case slice_how::keep_prefix:
        {
            keep_prefix_impl(n);
            break;
        }
        }
    }
};

//------------------------------------------------

template<class BufferSequence>
class slice_of<BufferSequence>::
    const_iterator
{
    using iter_type = typename
        slice_of::iter_type;

    iter_type it_;
    // VFALCO we could just point back to
    // the original sequence to save size
    std::size_t prefix_ = 0;
    std::size_t suffix_ = 0;
    std::size_t i_ = 0;
    std::size_t n_ = 0;

    friend class slice_of<BufferSequence>;

    const_iterator(
        iter_type it,
        std::size_t prefix__,
        std::size_t suffix__,
        std::size_t i,
        std::size_t n) noexcept
        : it_(it)
        , prefix_(prefix__)
        , suffix_(suffix__)
        , i_(i)
        , n_(n)
    {
        // n_ is the index of the end iterator
    }

public:
    using value_type = typename slice_of::value_type;
    using reference = value_type;
    using pointer = void;
    using difference_type = std::ptrdiff_t;
    using iterator_category =
        std::bidirectional_iterator_tag;
#if defined(__cpp_concepts) || defined(__cpp_lib_concepts)
    using iterator_concept = std::bidirectional_iterator_tag; // (since C++20)
#endif

    const_iterator() = default;

    bool
    operator==(
        const_iterator const& other) const noexcept
    {
        return
            it_     == other.it_ &&
            prefix_ == other.prefix_ &&
            suffix_ == other.suffix_ &&
            i_      == other.i_ &&
            n_      == other.n_;
    }

    bool
    operator!=(
        const_iterator const& other) const noexcept
    {
        return !(*this == other);
    }

    reference
    operator*() const noexcept
    {
        value_type v = *it_;
        using P = typename std::conditional<
            is_mutable_buffer_sequence<BufferSequence>::value,
            char*, char const*>::type;
        auto p = reinterpret_cast<P>(v.data());
        auto n = v.size();
        if(i_ == 0)
        {
            p += prefix_;
            n -= prefix_;
        }
        if(i_ == n_ - 1)
            n -= suffix_;
        return value_type(p, n);
    }

    const_iterator&
    operator++() noexcept
    {
        BOOST_ASSERT(i_ < n_);
        ++it_;
        ++i_;
        return *this;
    }

    const_iterator
    operator++(int) noexcept
    {
        auto temp = *this;
        ++(*this);
        return temp;
    }

    const_iterator&
    operator--() noexcept
    {
        BOOST_ASSERT(i_ > 0);
        --it_;
        --i_;
        return *this;
    }

    const_iterator
    operator--(int) noexcept
    {
        auto temp = *this;
        --(*this);
        return temp;
    }
};

//------------------------------------------------

template<class BufferSequence>
auto
slice_of<BufferSequence>::
begin() const noexcept ->
    const_iterator
{
    return const_iterator(
        this->begin_, prefix_, suffix_, 0, len_);
}

template<class BufferSequence>
auto
slice_of<BufferSequence>::
end() const noexcept ->
    const_iterator
{
    return const_iterator(
        this->end_, prefix_, suffix_, len_, len_);
}

//------------------------------------------------

// in-place modify  return value
// -----------------------------
// keep_prefix*     prefix
// keep_suffix      suffix
// remove_prefix*   sans_prefix
// remove_suffix    sans_suffix

/** Remove all but the first `n` bytes from a buffer sequence
*/
constexpr struct
{
    template<class BufferSequence>
    auto operator()(
        BufferSequence& bs,
        std::size_t n) const -> typename std::enable_if<
            is_const_buffer_sequence<BufferSequence>::value &&
            detail::has_tag_invoke<BufferSequence>::value>::type

    {
        tag_invoke(slice_tag{}, bs, slice_how::keep_prefix, n);
    }
} const keep_prefix{};

/** Remove all but the last `n` bytes from a buffer sequence
*/
constexpr struct
{
    template<class BufferSequence>
    auto operator()(
        BufferSequence& bs,
        std::size_t n) const -> typename std::enable_if<
            is_const_buffer_sequence<BufferSequence>::value &&
            detail::has_tag_invoke<BufferSequence>::value>::type
    {
        auto n0 = size(bs);
        if(n < n0)
            tag_invoke(slice_tag{}, bs, slice_how::remove_prefix, n0 - n);
    }
} const keep_suffix{};

/** Remove `n` bytes from the beginning of a buffer sequence
*/
constexpr struct
{
    template<class BufferSequence>
    auto operator()(
        BufferSequence& bs,
        std::size_t n) const -> typename std::enable_if<
            is_const_buffer_sequence<BufferSequence>::value &&
            detail::has_tag_invoke<BufferSequence>::value>::type
    {
        tag_invoke(slice_tag{}, bs, slice_how::remove_prefix, n);
    }
} const remove_prefix{};

/** Remove `n` bytes from the end of a buffer sequence
*/
constexpr struct
{
    template<class BufferSequence>
    auto operator()(
        BufferSequence& bs,
        std::size_t n) const -> typename std::enable_if<
            is_const_buffer_sequence<BufferSequence>::value &&
            detail::has_tag_invoke<BufferSequence>::value>::type
    {
        auto n0 = size(bs);
        if(n > 0)
        {
            if( n > n0)
                n = n0;
            tag_invoke(slice_tag{}, bs, slice_how::keep_prefix, n0 - n);
        }
    }
} const remove_suffix{};

//------------------------------------------------

/** Return a sequence representing the first `n` bytes of a buffer sequence
*/
constexpr struct
{
    template<class BufferSequence>
    auto operator()(
        BufferSequence const& bs,
        std::size_t n) const noexcept -> typename std::enable_if<
            is_const_buffer_sequence<BufferSequence>::value,
            slice_type<BufferSequence>>::type
    {
        slice_type<BufferSequence> result(bs);
        keep_prefix(result, n);
        return result;
    }
} prefix{};

/** Return a sequence representing the last `n` bytes of a buffer sequence
*/
constexpr struct
{
    template<class BufferSequence>
    auto operator()(
        BufferSequence const& bs,
        std::size_t n) const noexcept -> typename std::enable_if<
            is_const_buffer_sequence<BufferSequence>::value,
            slice_type<BufferSequence>>::type
    {
        slice_type<BufferSequence> result(bs);
        keep_suffix(result, n);
        return result;
    }
} suffix{};

/** Return a sequence representing all but the first `n` bytes of a buffer sequence
*/
constexpr struct
{
    template<class BufferSequence>
    auto operator()(
        BufferSequence const& bs,
        std::size_t n) const noexcept -> typename std::enable_if<
            is_const_buffer_sequence<BufferSequence>::value,
            slice_type<BufferSequence>>::type
    {
        slice_type<BufferSequence> result(bs);
        remove_prefix(result, n);
        return result;
    }
} sans_prefix{};

/** Return a sequence representing all but the last `n` bytes of a buffer sequence
*/
constexpr struct
{
    template<class BufferSequence>
    auto operator()(
        BufferSequence const& bs,
        std::size_t n) const noexcept -> typename std::enable_if<
            is_const_buffer_sequence<BufferSequence>::value,
            slice_type<BufferSequence>>::type
    {
        slice_type<BufferSequence> result(bs);
        remove_suffix(result, n);
        return result;
    }
} sans_suffix{};

} // buffers
} // boost

#endif
