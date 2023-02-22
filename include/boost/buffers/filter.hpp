//
// Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/buffers
//

#ifndef BOOST_BUFFERS_FILTER_HPP
#define BOOST_BUFFERS_FILTER_HPP

#include <boost/buffers/detail/config.hpp>
#include <boost/buffers/buffered_base.hpp>
#include <boost/buffers/const_buffer.hpp>
#include <boost/buffers/const_buffer_span.hpp>
#include <boost/buffers/mutable_buffer_span.hpp>
#include <boost/system/error_code.hpp>
#include <cstddef>

namespace boost {
namespace buffers {

/** A filter for buffers
*/
class BOOST_SYMBOL_VISIBLE
    filter
    : public buffered_base
{
public:
    /** The results of processing the filter.
    */
    struct results
    {
        /** The number of bytes produced in the output.

            This may be less than the total number
            of bytes available for writing in the
            destination buffers.
        */
        std::size_t out_bytes = 0;

        /** The number of bytes consumed from the input.

            This may be less than the total number
            of bytes available for reading in the
            source buffers.
        */
        std::size_t in_bytes = 0;

        /** The error, if any occurred.
        */
        system::error_code ec;

        /** True if there will be no more output.
        */
        bool finished = false;
    };

    /** Called to process the filter.

        @par Preconditions
        @ref init was called once before any
            calls to `process`
    */
    virtual
    results
    process(
        mutable_buffer out,
        const_buffer in,
        bool more) = 0;

    /** Called to process the filter.

        @par Preconditions
        @ref init was called once before any
            calls to `process`.
    */
    BOOST_BUFFERS_DECL
    virtual
    results
    process(
        mutable_buffer_span const& out,
        const_buffer_span const& in,
        bool more);

    /** Called to process the filter.

        @par Preconditions
        @ref init was called once before any
            calls to `process`.
    */
    template<
        class MutableBufferSequence,
        class ConstBufferSequence>
    results
    process(
        MutableBufferSequence const& out,
        ConstBufferSequence const& in,
        bool more);
};

//------------------------------------------------

/** Metafunction which determines if T is a filter

    @see
        @ref source.
*/
#ifdef BOOST_BUFFERS_DOCS
template<class T>
using is_filter = __see_below__;
#else
template<class T>
using is_filter =
    std::is_convertible<
        typename std::decay<T>::type*,
        filter*>;
#endif

} // buffers
} // boost

#include <boost/buffers/impl/filter.hpp>

#endif
