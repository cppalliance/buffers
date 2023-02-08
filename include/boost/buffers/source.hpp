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
#include <boost/system/error_code.hpp>
#include <cstddef>
#include <type_traits>

namespace boost {
namespace buffers {

/** An algorithm to produce data in buffers.

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

        @param bs The buffers to write into.
    */
    template<class MutableBufferSequence>
    results
    read(MutableBufferSequence const& bs);

#ifdef BOOST_BUFFERS_DOCS
protected:
#else
private:
#endif
    /** Implementation for producing data.

        This pure virtual function is called by
        the implementation and must be overriden
        by the derived class. It should attempt
        to place zero or more bytes of data into
        the span of buffer passed in, and return
        an appropriately set @ref results.

        @par Preconditions
        @code
        this->is_inited() && this->is_finished() == false
        @endcode

        @return The result of the operation.

        @param bs A span of mutable buffers.
    */
    BOOST_BUFFERS_DECL
    virtual
    results
    on_read(
        mutable_buffer_span bs) = 0;

private:
    BOOST_BUFFERS_DECL
    results read_impl(mutable_buffer_span);
    results read_impl(mutable_buffer const&);
    template<class T> results read_impl(T const&);
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
