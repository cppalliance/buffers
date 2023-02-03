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
#include <boost/buffers/mutable_buffer.hpp>
#include <boost/system/error_code.hpp>
#include <cstddef>

namespace boost {
namespace buffers {

/** A producer of buffered data.

    This interface abstracts the production of
    a finite stream of data, returned by writing
    into caller-provided buffers until the end
    of the stream is reached.

    @par Thread Safety
    Non-const member functions may not be
    called concurrently on the same instance.
*/
class BOOST_SYMBOL_VISIBLE
    source
    : public buffered_base
{
public:
    /** The results of producing data.
    */
    struct results
    {
        /** The number of bytes produced in the output.

            This may be less than the total number
            of bytes available for writing in the
            destination.
        */
        std::size_t bytes = 0;

        /** The error, if any occurred.
        */
        system::error_code ec;

        /** True if there will be no more output.
        */
        bool finished = false;
    };

    /** Produce data.

        This function produces zero or more
        bytes of data and places the result
        in the passed buffer.

        @par Preconditions
        @ref init was called once before any
            calls to `read_one`, and:
        @code
        dest != nullptr || size == 0
        @endcode

        @param dest A pointer to contiguous
            storage of at least `size` bytes.

        @param size The number of valid bytes
            of storage pointed to by `dest`.
    */
    virtual
    results
    read_one(
        void* dest,
        std::size_t size) = 0;

    /** Read data from the source.

        This function attempts to read data from
        the source into the specified array of
        mutable buffers. The base class
        implementation calls @ref read_one for
        each buffer in the array. Derived classes
        may override this function; for example,
        when knowing the total amount of space
        in the set of destination buffers is
        helpful.

        @param dest A pointer to an array of
            @ref mutable_buffer of at least
            `dest_len` valid elements.

        @param dest_len The number of valid
            elements pointed to by `dest`.
    */
    BOOST_BUFFERS_DECL
    virtual
    results
    read(
        mutable_buffer const* dest,
        std::size_t dest_len);

    /** Read data from the source.

        This function attempts to read data from
        the source into the specified mutable
        buffer sequence. It is implemented in
        terms of zero or more calls to @ref read
        with an array of mutable buffers. The
        size of this array will never be less
        than two.

        @par Preconditions
        @ref init was called once before any
            calls to read.

        @param dest The destination to write to.
    */
    template<class MutableBufferSequence>
    results
    read(MutableBufferSequence const& dest);
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
