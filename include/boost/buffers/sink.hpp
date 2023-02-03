//
// Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/buffers
//

#ifndef BOOST_BUFFERS_SINK_HPP
#define BOOST_BUFFERS_SINK_HPP

#include <boost/buffers/detail/config.hpp>
#include <boost/buffers/buffered_base.hpp>
#include <boost/buffers/const_buffer.hpp>
#include <boost/system/error_code.hpp>

namespace boost {
namespace buffers {

/** A consumer of buffered data.

    This interface abstracts the consumption of
    a finite stream of data, represented by
    zero or more caller provided, read-only
    buffers passed sequentially until there
    are no more buffers.

    @par Thread Safety
    Non-const member functions may not be
    called concurrently on the same instance.
*/
class BOOST_SYMBOL_VISIBLE
    sink
    : buffered_base
{
public:
    /** The results of consuming data.

        Partial success is possible.
    */
    struct results
    {
        /** The number of bytes consumed in the input.
        */
        std::size_t bytes = 0;

        /** The error, if any occurred.
        */
        system::error_code ec;
    };

    /** Consume data.

        This function consumes zero or more
        bytes of data from the passed buffer.

        @par Preconditions
        @ref init was called once before any
            calls to `read_one`, and:
        @code
        dest != nullptr || size == 0
        @endcode

        @param src A pointer to contiguous
            storage of at least `size` bytes.

        @param size The number of valid bytes
            of storage pointed to by `dest`.

        @param more `true` if there will be a
            subsequent call to `write_one`.
    */
    virtual
    results
    write_one(
        void const* src,
        std::size_t src_size,
        bool more) = 0;

    BOOST_BUFFERS_DECL
    virtual
    results
    write(
        const_buffer const* src,
        std::size_t src_len,
        bool more);

    template<class ConstBufferSequence>
    results
    write(
        ConstBufferSequence const& src,
        bool more);
};

//------------------------------------------------

/** Metafunction which determines if T is a sink

    @see
        @ref sink.
*/
#ifdef BOOST_BUFFERS_DOCS
template<class T>
using is_sink = __see_below__;
#else
template<class T>
using is_sink =
    std::is_convertible<
        typename std::decay<T>::type*,
        sink*>;
#endif

} // buffers
} // boost

#include <boost/buffers/impl/sink.hpp>

#endif
