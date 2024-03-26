//
// Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/buffers
//

#ifndef BOOST_BUFFERS_TYPE_TRAITS_HPP
#define BOOST_BUFFERS_TYPE_TRAITS_HPP

#include <boost/buffers/detail/config.hpp>
#include <boost/buffers/detail/type_traits.hpp>
#include <boost/core/data.hpp>
#include <type_traits>

namespace boost {
namespace buffers {

#ifndef BOOST_BUFFERS_DOCS
class const_buffer;
class mutable_buffer;
#endif

// https://www.boost.org/doc/libs/1_65_0/doc/html/boost_asio/reference/ConstBufferSequence.html

/** Determine if T is a ConstBuffers.
*/
#if BOOST_BUFFERS_DOCS
template<class T>
struct is_const_buffer_sequence
    : std::integral_constant<bool, ...>{};
#else

namespace detail
{
    template <class>
    struct is_buffer
    {
        static constexpr bool value = false;
    };

    template <>
    struct is_buffer<const_buffer>
    {
        static constexpr bool value = true;
    };

    template <>
    struct is_buffer<mutable_buffer>
    {
        static constexpr bool value = true;
    };

    template <class T>
    using buffers_ptr =
        decltype(boost::data(std::declval<T&>()));

    template <class T, class = void>
    struct buffers_data
    {
    };

    template <class T>
    struct buffers_data<
        T, typename std::enable_if<
            std::is_pointer<buffers_ptr<T>>::value
        >::type>
    {
        using type =
            typename std::remove_pointer<
                buffers_ptr<T>>::type;
    };

    template <class, class = void>
    struct buffers_has_data
    {
        static constexpr bool value = false;
    };

    template<class T>
    struct buffers_has_data<
        T, typename std::enable_if<
            std::is_trivial<
                typename buffers_data<T>::type
            >::value &&
            !std::is_const<
                typename buffers_data<T>::type
            >::value
        >::type>
    {
        static constexpr bool value = true;
    };

    template <class, class = void>
    struct buffers_has_cdata
    {
        static constexpr bool value = false;
    };

    template<class T>
    struct buffers_has_cdata<
        T, typename std::enable_if<
            std::is_trivial<
                typename buffers_data<T>::type
            >::value &&
            std::is_const<
                typename buffers_data<T>::type
            >::value
        >::type>
    {
        static constexpr bool value = true;
    };

    template<class T, class=void>
    struct buffers_has_size {
        static constexpr bool const value = false;
    };

    template<class T>
    struct buffers_has_size<T,
        typename std::enable_if<
            std::is_convertible<
                decltype(std::declval<T&>().size()),
                std::size_t
            >::value
    >::type>
    {
        static constexpr bool value = true;
    };

    template <class T>
    using buffers_uncvref = typename std::remove_cv<typename
        std::remove_reference<T>::type>::type;

    template<class T>
    struct is_mutable_contiguous_container
    {
        static constexpr bool value =
            !is_buffer<buffers_uncvref<T>>::value &&
            !std::is_array<buffers_uncvref<T>>::value &&
            buffers_has_data<T>::value &&
            buffers_has_size<T>::value;
    };

    template<class T, class = void>
    struct is_const_contiguous_container
    {
        static constexpr bool value =
            !is_buffer<buffers_uncvref<T>>::value &&
            !std::is_array<buffers_uncvref<T>>::value &&
            buffers_has_cdata<T>::value &&
            buffers_has_size<T>::value;
    };
}

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

#endif

/** Determine if T is a MutableBuffers.
*/
#if BOOST_BUFFERS_DOCS
template<class T>
struct is_mutable_buffer_sequence
    : std::integral_constant<bool, ...>{};
#else

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

#endif

//------------------------------------------------

/** Determine if T is a DynamicBuffer
*/
#if BOOST_BUFFERS_DOCS
template<class T>
struct is_dynamic_buffer
    : std::integral_constant<bool, ...>{};
#else

template<
    class T,
    class = void>
struct is_dynamic_buffer : std::false_type {};

template<class T>
struct is_dynamic_buffer<
    T, void_t<decltype(
        std::declval<std::size_t&>() =
            std::declval<T const&>().size()
        ,std::declval<std::size_t&>() =
            std::declval<T const&>().max_size()
        ,std::declval<std::size_t&>() =
            std::declval<T const&>().capacity()
        ,std::declval<T&>().commit(
            std::declval<std::size_t>())
        ,std::declval<T&>().consume(
            std::declval<std::size_t>())
    )
    ,typename std::enable_if<
        is_const_buffer_sequence<typename
            T::const_buffers_type>::value
        && is_mutable_buffer_sequence<typename
            T::mutable_buffers_type>::value
        >::type
    ,typename std::enable_if<
        std::is_same<decltype(
            std::declval<T const&>().data()),
            typename T::const_buffers_type>::value
        && std::is_same<decltype(
            std::declval<T&>().prepare(
                std::declval<std::size_t>())),
            typename T::mutable_buffers_type>::value
        >::type
    > > : std::true_type
{
};

/** Return the underlying buffer type of a sequence.
*/
template<class T>
using value_type = typename
    std::conditional<
        is_mutable_buffer_sequence<T>::value,
        mutable_buffer,
        const_buffer
            >::type;

#endif

} // buffers
} // boost

#endif
