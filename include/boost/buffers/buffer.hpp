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
#include <iterator>
#include <memory>
#include <type_traits>

// https://www.boost.org/doc/libs/1_65_0/doc/html/boost_asio/reference/ConstBufferSequence.html

// GCC 6 and earlier bug fixed after PR 42329
// ("deduction of template template argument via base class fails")
#if defined(__GNUC__) && ! defined(__clang__) && (__GNUC__ < 7)
#define BOOST_BUFFERS_GCC6_WORKAROUND
#endif

namespace boost {

namespace asio {
class const_buffer;
class mutable_buffer;
} // asio

namespace buffers {

#ifndef BOOST_BUFFERS_GCC6_WORKAROUND

class const_buffer;
class mutable_buffer;

namespace detail {

// this is designed to satisfy Asio's buffer constructors
template<class T, std::size_t Extent = (std::size_t)(-1)>
class basic_buffer
{
public:
    /** Return a pointer to the beginning of the memory region
    */
    constexpr auto
    data() const noexcept ->
        typename std::conditional<
            std::is_const<T>::value,
            void const*, void*>::type
    {
        return p_;
    }

    /** Return the number of valid bytes in the referenced memory region
    */
    constexpr std::size_t size() const noexcept
    {
        return n_;
    }

private:
    friend class buffers::const_buffer;
    friend class buffers::mutable_buffer;
    friend class asio::const_buffer;
    friend class asio::mutable_buffer;
    basic_buffer() = default;
    constexpr basic_buffer(T* p, std::size_t n) noexcept : p_(p), n_(n) {}
    constexpr basic_buffer<T, (std::size_t)(-1)> subspan(
        std::size_t, std::size_t = (std::size_t)(-1)) const noexcept;

    T* p_ = nullptr;
    std::size_t n_ = 0;
};

} // detail

#endif

//-----------------------------------------------

namespace detail {
namespace adl {

using std::begin;
template<class T>
using begin_type = decltype(begin(std::declval<T&>()));

using std::end;
template<class T>
using end_type = decltype(end(std::declval<T&>()));

// determine if T is a bidirectional range
// whose value type is convertible to B
template<class T, class B, class = void>
struct is_bidir_of : std::false_type
{
};

template<class T, class B>
struct is_bidir_of<T, B, detail::void_t<typename std::enable_if<
    std::is_convertible<decltype(*std::declval<begin_type<T const>>()), B>::value &&
    std::is_convertible<decltype(*std::declval<end_type<T const>>()), B>::value &&
    detail::is_bidirectional_iterator<begin_type<T const>>::value &&
    detail::is_bidirectional_iterator<end_type<T const>>::value &&
    std::is_same<
        decltype(begin(std::declval<T const>())),
        decltype(end  (std::declval<T const>()))>::value
    >::type > >
    : std::true_type
{
};

} // adl
} // detail

//------------------------------------------------

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
    remove_prefix,

    /// Indicates that the front of the buffer sequence should be preserved
    keep_prefix
};

//------------------------------------------------

#ifndef BOOST_BUFFERS_GCC6_WORKAROUND

/** Holds a contiguous range of modifiable bytes
*/
class mutable_buffer
    : public detail::basic_buffer<unsigned char>
{
public:
    /** Constructor.
    */
    mutable_buffer() = default;

    /** Constructor.
    */
    mutable_buffer(
        mutable_buffer const&) = default;

    /** Assignment.
    */
    mutable_buffer& operator=(
        mutable_buffer const&) = default;

    /** Constructor.
    */
    constexpr mutable_buffer(
        void* data, std::size_t size) noexcept
        : basic_buffer<unsigned char>(
            static_cast<unsigned char*>(data), size)
    {
    }

    /** Constructor
    */
    template<class MutableBuffer, class = typename std::enable_if<
        std::is_same<MutableBuffer, asio::mutable_buffer>::value>::type>
    constexpr mutable_buffer(
        MutableBuffer const& b) noexcept
        : basic_buffer<unsigned char>(
            static_cast<unsigned char*>(
                b.data()), b.size())
    {
    }

    /** Remove a prefix of the memory region

        If the requested number of bytes is larger than the current size,
        the resulting buffer will have size 0.

        @param n The number of bytes to remove.
    */
    mutable_buffer&
    operator+=(std::size_t n) noexcept
    {
        if( n > n_)
            n = n_;
        p_ += n;
        n_ -= n;
        return *this;
    }

    /** Remove a slice from the buffer
    */
    friend
    void
    tag_invoke(
        slice_tag const&,
        mutable_buffer& b,
        slice_how how,
        std::size_t n) noexcept
    {
        b.do_slice(how, n);
    }

private:
    void do_slice(
        slice_how how, std::size_t n) noexcept
    {
        switch(how)
        {
        case slice_how::remove_prefix:
            *this += n;
            return;

        case slice_how::keep_prefix:
            if( n < n_)
                n_ = n;
            return;
        }
    }
};

//------------------------------------------------

/** Holds a contiguous range of unmodifiable bytes
*/
class const_buffer
    : public detail::basic_buffer<unsigned char const>
{
public:
    /** Constructor
    */
    const_buffer() = default;

    /** Constructor
    */
    const_buffer(const_buffer const&) = default;

    /** Assignment

        @par Postconditions
        @code
        this->data() == other.data() && this->size() == other.size()
        @endcode
    */
    const_buffer& operator=(
        const_buffer const& other) = default;

    /** Constructor
    */
    constexpr const_buffer(
        void const* data, std::size_t size) noexcept
        : basic_buffer<unsigned char const>(
            static_cast<unsigned char const*>(data), size)
    {
    }

    /** Constructor
    */
    constexpr const_buffer(
        mutable_buffer const& b) noexcept
        : basic_buffer<unsigned char const>(
            static_cast<unsigned char const*>(b.data()), b.size())
    {
    }

    /** Constructor
    */
    template<class ConstBuffer, class = typename std::enable_if<
        std::is_same<ConstBuffer, asio::const_buffer>::value ||
        std::is_same<ConstBuffer, asio::mutable_buffer>::value>::type>
    constexpr const_buffer(
        ConstBuffer const& b) noexcept
        : basic_buffer<unsigned char const>(
            static_cast<unsigned char const*>(
                b.data()), b.size())
    {
    }

    /** Remove a prefix of the memory region

        If the requested number of bytes is larger than the current size,
        the resulting buffer will have size 0.

        @param n The number of bytes to remove.
    */
    const_buffer&
    operator+=(std::size_t n) noexcept
    {
        if( n > n_)
            n = n_;
        p_ += n;
        n_ -= n;
        return *this;
    }

    /** Remove a slice from the buffer
    */
    friend
    void
    tag_invoke(
        slice_tag const&,
        const_buffer& b,
        slice_how how,
        std::size_t n) noexcept
    {
        b.do_slice(how, n);
    }

private:
    void do_slice(
        slice_how how, std::size_t n) noexcept
    {
        switch(how)
        {
        case slice_how::remove_prefix:
            *this += n;
            return;

        case slice_how::keep_prefix:
            if( n < n_)
                n_ = n;
            return;
        }
    }
};

#else

template<class T, std::size_t Extent = (std::size_t)(-1)>
class basic_buffer
{
    using pointer = typename std::conditional<
        std::is_const<T>::value, void const*, void*>::type;
public:
    basic_buffer() = default;
    basic_buffer(basic_buffer const&) = default;
    basic_buffer& operator=(basic_buffer const& other) = default;
    constexpr auto data() const noexcept -> pointer { return p_; }
    constexpr std::size_t size() const noexcept { return n_; }
    constexpr basic_buffer(pointer p, std::size_t n) noexcept
        : p_(p) , n_(n) {}
    template<class U, std::size_t E, class = typename std::enable_if<
        std::is_const<T>::value && ! std::is_const<U>::value>::type>
    constexpr basic_buffer(basic_buffer<U,E> b) noexcept
        : p_(b.data()), n_(b.size()) {}
    template<class Buffer, class = typename std::enable_if<
        std::is_same<Buffer, asio::mutable_buffer>::value ||
        (std::is_same<Buffer, asio::const_buffer>::value &&
            std::is_const<T>::value)>::type>
    constexpr basic_buffer(Buffer b) noexcept
        : p_(b.data()), n_(b.size()) {}
    basic_buffer& operator+=(std::size_t n) noexcept
    {
        if( n > n_)
            n = n_;
        p_ = static_cast<T*>(p_) + n;
        n_ -= n;
        return *this;
    }
    friend void tag_invoke(slice_tag const&, basic_buffer& b,
        slice_how how, std::size_t n) noexcept
    {
        switch(how)
        {
        case slice_how::remove_prefix:
            b += n;
            return;

        case slice_how::keep_prefix:
            if(n < b.n_)
                b.n_ = n;
            return;
        }
    }

private:
    friend class asio::const_buffer;
    friend class asio::mutable_buffer;
    constexpr basic_buffer<T, (std::size_t)(-1)>
    subspan(std::size_t, std::size_t = (std::size_t)(-1)) const noexcept;

    pointer p_ = nullptr;
    std::size_t n_ = 0;
};

using mutable_buffer = basic_buffer<unsigned char>;
using const_buffer = basic_buffer<unsigned char const>;

#endif

//------------------------------------------------------------------------------

/** Return an iterator pointing to the first element of a buffer sequence

    This function returns an iterator to the beginning of the range denoted by
    `t`. While this works for any valid range, it is provided for convenience
    when using C++17 and earlier, where `std::ranges::begin` is unavailable.

    @par Constraints
    @code
    requires std::ranges::range<T>
    @endcode

    @param t The buffer sequence
*/
constexpr struct begin_mrdocs_workaround_t
{
    template<
        class ConvertibleToBuffer
        ,class = typename std::enable_if<
            std::is_convertible<ConvertibleToBuffer, const_buffer>::value ||
            std::is_convertible<ConvertibleToBuffer, mutable_buffer>::value
        >::type
    >
    auto operator()(ConvertibleToBuffer const& b) const noexcept -> ConvertibleToBuffer const*
    {
        return std::addressof(b);
    }

    template<
        class BufferSequence
        ,class = typename std::enable_if<
            ! std::is_convertible<BufferSequence const*, const_buffer const*>::value &&
            ! std::is_convertible<BufferSequence const*, mutable_buffer const*>::value,
            detail::void_t< detail::adl::begin_type<BufferSequence const> > >::type
    >
    auto operator()(BufferSequence const& bs) const noexcept
#if ! (__cpp_decltype_auto >= 201304)
        -> detail::adl::begin_type<BufferSequence const>
#endif
    {
        using std::begin;
        return begin(bs);
    }

    template<
        class BufferSequence
        ,class = typename std::enable_if<
            ! std::is_convertible<BufferSequence const*, const_buffer const*>::value &&
            ! std::is_convertible<BufferSequence const*, mutable_buffer const*>::value,
            detail::void_t< detail::adl::begin_type<BufferSequence> > >::type
    >
    auto operator()(BufferSequence& bs) const noexcept
#if ! (__cpp_decltype_auto >= 201304)
        -> detail::adl::begin_type<BufferSequence>
#endif
    {
        using std::begin;
        return begin(bs);
    }
} begin {};

//------------------------------------------------------------------------------

/** Return an iterator to the end of the buffer sequence

    This function returns an iterator to the end of the range denoted by
    `t`. While this works for any valid range, it is provided for convenience
    when using C++17 and earlier, where `std::ranges::end` is unavailable.

    @par Constraints
    @code
    requires std::ranges::range<T>
    @endcode

    @param t The buffer sequence
*/
constexpr struct end_mrdocs_workaround_t
{
    template<
        class ConvertibleToBuffer
        ,class = typename std::enable_if<
            std::is_convertible<ConvertibleToBuffer, const_buffer>::value ||
            std::is_convertible<ConvertibleToBuffer, mutable_buffer>::value
        >::type
    >
    auto operator()(ConvertibleToBuffer const& b) const noexcept -> ConvertibleToBuffer const*
    {
        return std::addressof(b) + 1;
    }

    template<
        class BufferSequence
        ,class = typename std::enable_if<
            ! std::is_convertible<BufferSequence const*, const_buffer const*>::value &&
            ! std::is_convertible<BufferSequence const*, mutable_buffer const*>::value,
            detail::void_t< detail::adl::end_type<BufferSequence const> > >::type
    >
    auto operator()(BufferSequence const& bs) const noexcept
#if ! (__cpp_decltype_auto >= 201304)
        -> detail::adl::end_type<BufferSequence const>
#endif
    {
        using std::end;
        return end(bs);
    }

    template<
        class BufferSequence
        ,class = typename std::enable_if<
            ! std::is_convertible<BufferSequence const*, const_buffer const*>::value &&
            ! std::is_convertible<BufferSequence const*, mutable_buffer const*>::value,
            detail::void_t< detail::adl::end_type<BufferSequence> > >::type
    >
    auto operator()(BufferSequence& bs) const noexcept
#if ! (__cpp_decltype_auto >= 201304)
        -> detail::adl::end_type<BufferSequence>
#endif
    {
        using std::end;
        return end(bs);
    }
} end {};

//------------------------------------------------

/** Determine if T is a ConstBufferSequence.
*/
/** @{ */
template<class T, class = void>
struct is_const_buffer_sequence
    : std::false_type
{
};

template<class T>
struct is_const_buffer_sequence<T>
    : std::integral_constant<bool,
        detail::adl::is_bidir_of<typename std::remove_cv<typename
            std::remove_reference<T>::type>::type, const_buffer>::value ||
        std::is_convertible<T, const_buffer>::value>
{
};

#if defined(__cpp_variable_templates) && __cpp_variable_templates >= 201304
template<class T>
constexpr bool is_const_buffer_sequence_v =
    is_const_buffer_sequence<T>::value;
#endif
/** @} */

//------------------------------------------------

/** Determine if T is a MutableBufferSequence.
*/
/** @{ */
template<class T, class = void>
struct is_mutable_buffer_sequence : std::false_type
{
};

template<class T>
struct is_mutable_buffer_sequence<T>
    : std::integral_constant<bool,
        detail::adl::is_bidir_of<typename std::remove_cv<typename
            std::remove_reference<T>::type>::type, mutable_buffer>::value ||
        std::is_convertible<T, mutable_buffer>::value>
{
};

#if defined(__cpp_variable_templates) && __cpp_variable_templates >= 201304
template<class T>
constexpr bool is_mutable_buffer_sequence_v =
    is_mutable_buffer_sequence<T>::value;
#endif
/** @} */

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

    This function returns the sum of the number of bytes in each contiguous
    buffer contained in the range or value. This is different from the length
    of the sequence returned by `std::ranges::size(t)`

    @par Constraints
    @code
    is_const_buffer_sequence_v<T> || std::is_convertible<T,const_buffer>
    @endcode

    @par Example
    @code
    template<class ConstBufferSequence>
    bool is_small( ConstBufferSequence const& bs ) noexcept
    {
        return size(bs) < 100;
    }
    @endcode
*/
constexpr struct size_mrdocs_workaround_t
{
    template<class ConstBufferSequence>
    constexpr auto operator()(
        ConstBufferSequence const& bs) const noexcept ->
            typename std::enable_if<
                is_const_buffer_sequence<ConstBufferSequence>::value,
                std::size_t>::type
    {
        return tag_invoke(size_tag{}, bs);
    }
} size {};

//-----------------------------------------------

namespace detail {

template<class It>
auto
length_impl(It first, It last, int)
    -> decltype(static_cast<std::size_t>(last - first))
{
    return static_cast<std::size_t>(last - first);
}

template<class It>
std::size_t
length_impl(It first, It last, long)
{
    std::size_t n = 0;
    while(first != last)
    {
        ++first;
        ++n;
    }
    return n;
}

} // detail

/** Return the number of elements in a buffer sequence.
*/
template<class ConstBufferSequence>
std::size_t
length(ConstBufferSequence const& bs)
{
    return detail::length_impl(
        buffers::begin(bs), buffers::end(bs), 0);
}

} // buffers
} // boost

#endif
