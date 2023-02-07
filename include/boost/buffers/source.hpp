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
    bool finished_ = false;

public:
    /** The results of producing data.
    */
    struct results
    {
        /** The error, if any occurred.
        */
        system::error_code ec;

        /** The number of bytes produced in the output.

            This may be less than the total number
            of bytes available for writing in the
            destination.
        */
        std::size_t bytes = 0;

        /** True if there will be no more output.
        */
        bool finished = false;

        /** Accumulate results.
        */
        results&
        operator+=(
            results const& rv);
    };

    /** Return true if the source is finished.
    */
    bool
    is_finished() const noexcept
    {
        return finished_;
    }

    /** Produce data.

        This function produces up to `size`
        bytes of data and places the result
        in the contiguous storage pointed to
        by `dest`.
        Afterwards, if there is no more data
        remaining then @ref is_finished will
        return `true`.
        <br>
        Once the end of the data is encountered,
        this function must not be called again or
        else an exception is thrown.

        @throws std::logic_error @ref init not called.

        @throws std::logic_error @ref is_finished returns `true`.

        @return The result of the operation.

        @param b The buffer to write into.
    */
    results
    read(mutable_buffer b);

    /** Produce data.

        This function produces data and places
        the result into zero or more contiguous
        storage areas described by an array of
        mutable buffers.
        Afterwards, if there is no more data
        remaining then @ref is_finished will
        return `true`.
        <br>
        Once the end of the data is encountered,
        this function must not be called again or
        else an exception is thrown.

        @throws std::logic_error @ref init not called.

        @throws std::logic_error @ref is_finished returns `true`.

        @return The result of the operation.

        @param dest A pointer to an array of
            @ref mutable_buffer of at least
            `dest_len` valid elements.

        @param dest_len The number of valid
            elements pointed to by `dest`.
    */
    results
    read(
        mutable_buffer const* dest,
        std::size_t dest_len);

    /** Produce data.

        This function produces data and
        places the result into the mutable
        buffer sequence `b`.
        Afterwards, if there is no more data
        remaining then @ref is_finished will
        return `true`.
        <br>
        Once the end of the data is encountered,
        this function must not be called again or
        else an exception is thrown.

        @throws std::logic_error @ref init not called.

        @throws std::logic_error @ref is_finished returns `true`.

        @return The result of the operation.

        @param dest A mutable buffer sequence.
    */
    template<class MutableBufferSequence>
    results
    read(MutableBufferSequence const& dest);

#ifdef BOOST_BUFFERS_DOCS
protected:
#else
private:
#endif
    /** Implementation for producing data.

        This pure virtual function is called by
        the implementation. Derived classes should
        place zero or more bytes of data into the
        buffer pointed to by `dest`.
        <br>
        This will will only be called if @ref init
        was called and the source is not finished.

        @par Preconditions
        @code
        this->is_inited() && ! this->is_finished()
        @endcode

        @return The result of the operation.

        @param dest A pointer to contiguous storage.

        @param size The number of valid bytes
            of storage pointed to by `dest`.
    */
    BOOST_BUFFERS_DECL
    virtual
    results
    do_read_one(
        void* dest,
        std::size_t size) = 0;

    /** Implementation for producing data.

        This virtual function is called by the
        implementation. The default implementation
        simply calls @ref do_read_one for each
        mutable buffer in the array. Derived
        classes may override this function to
        optimize the case where a range of
        buffers is presented for reading.
        <br>
        This will will only be called if @ref init
        was called and the source is not finished.

        @par Preconditions
        @code
        this->is_inited() && ! this->is_finished()
        @endcode

        @return The result of the operation.

        @param dest An array of mutable buffers.

        @param dest_len The number of elements
            in the array pointed to by `dest`.
    */
    BOOST_BUFFERS_DECL
    virtual
    results
    do_read(
        mutable_buffer const* dest,
        std::size_t dest_len);
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
