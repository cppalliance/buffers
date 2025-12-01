//
// Copyright (c) 2025 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/buffers
//

#ifndef BOOST_BUFFERS_DATA_SOURCE_HPP
#define BOOST_BUFFERS_DATA_SOURCE_HPP

#include <boost/buffers/detail/config.hpp>
#include <boost/buffers/detail/except.hpp>
#include <boost/buffers/buffer.hpp>
#include <boost/system/error_code.hpp>

namespace boost {
namespace buffers {

/** Metafunction to detect if a type is a data source.

    A data source presents a binary object as a constant buffer sequence.

    @par Requirements
    @code
    struct DataSource
    {
        DataSource( DataSource&& ) noexcept;
        ConstBufferSequence data() const noexcept;
    };
    @endcode

    Where `buffers::is_const_buffer_sequence<ConstBufferSequence>::value` is `true`.
*/
template<class T, class = void>
struct is_data_source
    : std::false_type
{
};

template<class T>
struct is_data_source<T, detail::void_t<
    decltype(std::declval<T const&>().data())>>
    : std::integral_constant<bool,
        std::is_nothrow_move_constructible<T>::value &&
        buffers::is_const_buffer_sequence<
            decltype(std::declval<T const&>().data())>::value>
{
};

} // buffers
} // boost

#endif
