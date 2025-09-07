//
// Copyright (c) 2025 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/buffers
//

#ifndef BOOST_BUFFERS_BUFFER_HPP
#define BOOST_BUFFERS_BUFFER_HPP

#include <boost/buffers/detail/config.hpp>
#include <boost/buffers/detail/type_traits.hpp>
#include <cstddef>
#include <type_traits>

namespace boost {
namespace buffers {

/** size tag for `tag_invoke`

    This type is used in overloads of `tag_invoke`
    for user-defined types to customize the `size()`
    algorithm.
*/
struct size_tag {};

/** slice tag for `tag_invoke`

    This type is used in overloads of `tag_invoke`
    for user-defined types to customize the slicing
    algorithms.
*/
struct slice_tag {};

/** slice constants for slice customization

    This defines the possible values passed to
    overloads of `tag_invoke` for user-defined
    types which customize the slicing algorithms.
*/
enum class slice_how
{
    /// Indicates that the front of the buffer sequence should be trimmed
    trim_front,

    /// Indicates that the back of the buffer sequence should be trimmed
    trim_back,

    /// Indicates that the front of the buffer sequence should be preserved
    keep_front,

    /// Indicates that the back of the buffer sequence should be preserved
    keep_back
};

//------------------------------------------------

/** Holds a contiguous range of modifiable bytes
*/
class mutable_buffer
{
    unsigned char* p_ = nullptr;
    std::size_t n_ = 0;

public:
    /** Constructor.
    */
    mutable_buffer() = default;

    /** Constructor.
    */
    mutable_buffer(
        mutable_buffer const&) = default;

    /** Constructor.
    */
    mutable_buffer(
        void* data,
        std::size_t size) noexcept
        : p_(static_cast<unsigned char*>(data))
        , n_(size)
    {
    }

    /** Assignment.
    */
    mutable_buffer& operator=(
        mutable_buffer const&) = default;

    // conversion to boost::asio::mutable_buffer
    template<
        class T
        , class = typename std::enable_if<
            std::is_constructible<T,
                void*, std::size_t>::value
            && ! std::is_same<T, mutable_buffer>::value
            //&& ! std::is_same<T, const_buffer>::value
        >::type
    >
    operator T() const noexcept
    {
        return T{ data(), size() };
    }

    void*
    data() const noexcept
    {
        return p_;
    }

    std::size_t
    size() const noexcept
    {
        return n_;
    }

    mutable_buffer&
    operator+=(std::size_t n) noexcept
    {
        if( n > n_)
            n = n_;
        p_ += n;
        n_ -= n;
        return *this;
    }

    friend
    void
    tag_invoke(
        slice_tag const&,
        mutable_buffer& b,
        slice_how how,
        std::size_t n) noexcept
    {
        switch(how)
        {
        case slice_how::trim_front:
            b += n;
            return;

        case slice_how::trim_back:
            if( n > b.n_)
                n = b.n_;
            b.n_ -= n;
            return;

        case slice_how::keep_front:
            if(n < b.n_)
                b.n_ = n;
            return;

        case slice_how::keep_back:
            if(n < b.n_)
            {
                b.p_ += b.n_ - n;
                b.n_ = n;
            }
            return;
        }
    }
};

//------------------------------------------------

/** Holds a contiguous range of unmodifiable bytes
*/
class const_buffer
{
    unsigned char const* p_ = nullptr;
    std::size_t n_ = 0;

public:
    /** Constructor.
    */
    const_buffer() = default;

    /** Constructor.
    */
    const_buffer(
        void const* data,
        std::size_t size) noexcept
        : p_(static_cast<
            unsigned char const*>(data))
        , n_(size)
    {
    }

    /** Constructor.
    */
    const_buffer(
        const_buffer const&) = default;

    /** Constructor.
    */
    const_buffer(
        mutable_buffer const& b) noexcept
        : p_(static_cast<
            unsigned char const*>(b.data()))
        , n_(b.size())
    {
    }

    /** Assignment.
    */
    const_buffer& operator=(
        const_buffer const&) = default;

    // conversion to boost::asio::const_buffer
    template<
        class T
        , class = typename std::enable_if<
            std::is_constructible<T,
                void const*, std::size_t>::value &&
            ! std::is_same<T, mutable_buffer>::value &&
            ! std::is_same<T, const_buffer>::value
        >::type
    >
    operator T() const noexcept
    {
        return T{ data(), size() };
    }

    void const*
    data() const noexcept
    {
        return p_;
    }

    std::size_t
    size() const noexcept
    {
        return n_;
    }

    const_buffer&
    operator+=(std::size_t n) noexcept
    {
        if( n > n_)
            n = n_;
        p_ += n;
        n_ -= n;
        return *this;
    }

    friend
    void
    tag_invoke(
        slice_tag const&,
        const_buffer& b,
        slice_how how,
        std::size_t n) noexcept
    {
        switch(how)
        {
        case slice_how::trim_front:
            b += n;
            return;

        case slice_how::trim_back:
            if( n > b.n_)
                n = b.n_;
            b.n_ -= n;
            return;

        case slice_how::keep_front:
            if(n < b.n_)
                b.n_ = n;
            return;

        case slice_how::keep_back:
            if(n < b.n_)
            {
                b.p_ += b.n_ - n;
                b.n_ = n;
            }
            return;
        }
    }
};

//------------------------------------------------------------------------------

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

// https://www.boost.org/doc/libs/1_65_0/doc/html/boost_asio/reference/ConstBufferSequence.html

//------------------------------------------------

/** Determine if T is a ConstBufferSequence.
*/
template<class T, class = void>
struct is_const_buffer_sequence
    : std::false_type
{
};

template<class T>
struct is_const_buffer_sequence<T const>
    : is_const_buffer_sequence<typename
        std::decay<T>::type>
{
};

template<class T>
struct is_const_buffer_sequence<T const&>
    : is_const_buffer_sequence<typename
        std::decay<T>::type>
{
};

template<class T>
struct is_const_buffer_sequence<T&>
    : is_const_buffer_sequence<typename
        std::decay<T>::type>
{
};

template<>
struct is_const_buffer_sequence<
    const_buffer>
    : std::true_type
{
};

template<>
struct is_const_buffer_sequence<
    mutable_buffer>
    : std::true_type
{
};

template<class T>
struct is_const_buffer_sequence<T, void_t<
    typename std::enable_if<
        (std::is_same<const_buffer, typename 
            T::value_type>::value
        || std::is_same<mutable_buffer, typename
            T::value_type>::value
            ) &&
        detail::is_bidirectional_iterator<typename
            T::const_iterator>::value &&
        std::is_same<typename
            T::const_iterator, decltype(
            std::declval<T const&>().begin())
                >::value &&
        std::is_same<typename
            T::const_iterator, decltype(
            std::declval<T const&>().end())
                >::value && (
        std::is_same<const_buffer, typename
            std::remove_const<typename
                std::iterator_traits<
                    typename T::const_iterator
                        >::value_type>::type
                >::value ||
        std::is_same<mutable_buffer, typename
            std::remove_const<typename
                std::iterator_traits<
                    typename T::const_iterator
                        >::value_type>::type
                >::value)
        // VFALCO This causes problems when the
        // trait is used to constrain ctors
        // && std::is_move_constructible<T>::value
            >::type
    > > : std::true_type
{
};

//------------------------------------------------

/** Determine if T is a MutableBufferSequence.
*/
template<class T, class = void>
struct is_mutable_buffer_sequence : std::false_type
{
};

template<class T>
struct is_mutable_buffer_sequence<T const>
    : is_mutable_buffer_sequence<typename
        std::decay<T>::type>
{
};

template<class T>
struct is_mutable_buffer_sequence<T const&>
    : is_mutable_buffer_sequence<typename
        std::decay<T>::type>
{
};

template<class T>
struct is_mutable_buffer_sequence<T&>
    : is_mutable_buffer_sequence<typename
        std::decay<T>::type>
{
};

template<>
struct is_mutable_buffer_sequence<
    mutable_buffer>
    : std::true_type
{
};

template<class T>
struct is_mutable_buffer_sequence<T, void_t<
    typename std::enable_if<
        std::is_same<mutable_buffer, typename
            T::value_type>::value &&
        detail::is_bidirectional_iterator<typename
            T::const_iterator>::value &&
        std::is_same<typename
            T::const_iterator, decltype(
            std::declval<T const&>().begin())
                >::value &&
        std::is_same<typename
            T::const_iterator, decltype(
            std::declval<T const&>().end())
                >::value &&
        std::is_same<mutable_buffer, typename
            std::remove_const<typename
                std::iterator_traits<
                    typename T::const_iterator
                        >::value_type>::type
                >::value
        // VFALCO This causes problems when the
        // trait is used to constrain ctors
        // && std::is_move_constructible<T>::value
            >::type
    >> : std::true_type
{
};

//------------------------------------------------------------------------------

template<class ConstBufferSequence>
std::size_t
tag_invoke(
    size_tag const&,
    ConstBufferSequence const& bs) noexcept
{
    std::size_t n = 0;
    auto const e = end(bs);
    for(auto it = begin(bs); it != e; ++it)
        n += const_buffer(*it).size();
    return n;
}

/** Return the total number of bytes in a buffer sequence

    @par Constraints
    @code
    is_const_buffer_sequence< ConstBufferSequence >::value == true
    @endcode
*/
constexpr struct
{
    template<class ConstBufferSequence>
    auto
    operator()(
        ConstBufferSequence const& bs) const noexcept ->
            typename std::enable_if<
            is_const_buffer_sequence<ConstBufferSequence>::value,
            std::size_t>::type
    {
        return tag_invoke(size_tag{}, bs);
    }
} size {};

} // buffers
} // boost

#endif
