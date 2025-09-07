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

/** Remove from the front of a buffer sequence
*/
constexpr struct
{
    template<
        class BufferSequence,
        class = typename std::enable_if<
            detail::has_tag_invoke<BufferSequence>::value>::type
    >
    void operator()(
        BufferSequence& bs,
        std::size_t n) const
    {
        tag_invoke(slice_tag{}, bs,
            slice_how::trim_front, n);
    }
} const trim_front{};

/** Remove from the back from a buffer sequence
*/
constexpr struct
{
    template<
        class BufferSequence,
        class = typename std::enable_if<
            detail::has_tag_invoke<BufferSequence>::value>::type
    >
    void operator()(
        BufferSequence& bs,
        std::size_t n) const
    {
        tag_invoke(slice_tag{}, bs,
            slice_how::trim_back, n);
    }
} const trim_back{};

/** Remove all but the front from a buffer sequence
*/
constexpr struct
{
    template<
        class BufferSequence,
        class = typename std::enable_if<
            detail::has_tag_invoke<BufferSequence>::value>::type
    >
    void operator()(
        BufferSequence& bs,
        std::size_t n) const
    {
        tag_invoke(slice_tag{}, bs,
            slice_how::keep_front, n);
    }
} const keep_front{};

/** Remove all but the back from a buffer sequence
*/
constexpr struct
{
    template<
        class BufferSequence,
        class = typename std::enable_if<
            detail::has_tag_invoke<BufferSequence>::value>::type
    >
    void operator()(
        BufferSequence& bs,
        std::size_t n) const
    {
        tag_invoke(slice_tag{}, bs,
            slice_how::keep_back, n);
    }
} const keep_back{};

//------------------------------------------------

/** A wrapper enabling a buffer sequence to be consumed
*/
template<class BufferSequence>
class slice_of
{
    // VFALCO Need to fix this
    //using iter_type = decltype(
        //std::declval<BufferSequence&>().begin());
    using iter_type = typename
        BufferSequence::const_iterator;

    BufferSequence bs_;
    iter_type begin_;
    iter_type end_;
    std::size_t len_ = 0;       // length of bs_
    std::size_t size_ = 0;      // total bytes
    std::size_t prefix_ = 0;    // used prefix bytes
    std::size_t suffix_ = 0;    // used suffix bytes

    // If you get a compile error here it
    // means that your type does not meet
    // the requirements.
    static_assert(
        is_const_buffer_sequence<BufferSequence>::value,
        "Type requirements not met.");

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
    trim_front(
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
    trim_back(
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
    keep_front(
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
        trim_back(size_ - n);
    }

    void
    keep_back(
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
        trim_front(size_ - n);
    }

    void
    slice_impl(
        slice_how how,
        std::size_t n)
    {
        switch(how)
        {
        case slice_how::trim_front:
        {
            trim_front(n);
            break;
        }
        case slice_how::trim_back:
        {
            trim_back(n);
            break;
        }
        case slice_how::keep_front:
        {
            keep_front(n);
            break;
        }
        case slice_how::keep_back:
        {
            keep_back(n);
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

} // buffers
} // boost

#endif
