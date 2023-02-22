//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/http_proto
//

#ifndef BOOST_BUFFERS_SOURCE_HPP
#define BOOST_BUFFERS_SOURCE_HPP

#include <boost/buffers/detail/config.hpp>
#include <boost/buffers/buffered_base.hpp>
#include <boost/buffers/mutable_buffer_span.hpp>
#include <boost/buffers/type_traits.hpp>
#include <boost/system/error_code.hpp>
#include <cstddef>
#include <type_traits>

namespace boost {
namespace buffers {

/** An algorithm for producing buffers of data.

    This interface abstracts the production of
    a finite stream of data, returned by writing
    into caller-provided buffers until there
    is no more output data.

    @par Thread Safety
    Non-const member functions may not be
    called concurrently on the same instance.
*/
struct BOOST_SYMBOL_VISIBLE
    source
    : buffered_base
{
    /** The results of producing data.
    */
    struct results
    {
        /** The error, if any occurred.
        */
        system::error_code ec;

        /** The number of bytes produced in the output.
        */
        std::size_t bytes = 0;

        /** True if there will be no more output.
        */
        bool finished = false;

        /** Accumulate results.
        */
        results&
        operator+=(
            results const& rv) noexcept;
    };

    /** Produce data.

        This function attempts to read from the
        source, placing the data into the given
        mutable buffer sequence.
        The return value indicates the number of
        bytes placed into the buffers, the error
        if any occurred, and a `bool` indicating
        whether or not there is more data
        remaining in the source.

        @par Preconditions
        @li @ref init was called, and
        @li There is more data remaining.

        @return The result of the operation.

        @param bs The buffers to use.
            Each buffer in the sequence will
            be filled completely before data
            is placed in the next buffer.
    */
    template<class MutableBufferSequence>
    results
    read(MutableBufferSequence const& bs)
    {
        static_assert(
            is_mutable_buffer_sequence<
                MutableBufferSequence>::value,
            "Type requirements not met");

        return read_impl(bs);
    }

#ifdef BOOST_BUFFERS_DOCS
protected:
#else
private:
#endif
    /** Derived class override.

        This pure virtual function is called by
        the implementation and must be overriden.
        The callee should attempt to place data
        into the given mutable buffer.
        The return value must be set to indicate
        the number of bytes placed into the
        buffers, the error if any occurred,
        and a `bool` indicating whether or
        not there is more data remaining
        in the source.

        @par Preconditions
        @li @ref init was called, and
        @li There is more data remaining.

        @return The result of the operation.

        @param b The buffer to use.
            If this is not filled completely,
            then the result must indicate failure
            or that no more data remains (or both).
    */
    BOOST_BUFFERS_DECL
    virtual
    results
    on_read(
        mutable_buffer b) = 0;

    /** Derived class override.

        This pure virtual function is called by
        the implementation and must be overriden.
        The callee should attempt to place data
        into the given mutable buffer sequence.
        The return value must be set to indicate
        the number of bytes placed into the
        buffers, the error if any occurred,
        and a `bool` indicating whether or
        not there is more data remaining
        in the source.

        @par Preconditions
        @li @ref init was called, and
        @li There is more data remaining.

        @return The result of the operation.

        @param bs The buffer sequence to use.
            Each buffer in the sequence must
            be filled completely before data
            is placed in the next buffer.
            If the buffers are not filled
            completely, then the result must
            indicate failure or that no more
            data remains (or both).       
    */
    BOOST_BUFFERS_DECL
    virtual
    results
    on_read(
        mutable_buffer_span bs);

private:
    results
    read_impl(
        mutable_buffer const& b)
    {
        return on_read(b);
    }

    results
    read_impl(
        mutable_buffer_span const& bs)
    {
        return on_read(bs);
    }

    template<class T>
    results
    read_impl(T const&);
};

//------------------------------------------------

/** Metafunction which determines if T is a source

    @see
        @ref source.
*/
#ifdef BOOST_BUFFERS_DOCS
template<class T>
using is_source = __see_below__;
#else
template<class T>
using is_source =
    std::is_convertible<
        typename std::decay<T>::type*,
        source*>;
#endif

} // buffers
} // boost

#include <boost/buffers/impl/source.hpp>

#endif
