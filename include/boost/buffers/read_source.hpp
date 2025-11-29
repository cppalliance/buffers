//
// Copyright (c) 2025 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/buffers
//

#ifndef BOOST_BUFFERS_READ_SOURCE_HPP
#define BOOST_BUFFERS_READ_SOURCE_HPP

#include <boost/buffers/detail/config.hpp>
#include <boost/buffers/detail/except.hpp>
#include <boost/buffers/buffer.hpp>
#include <boost/system/error_code.hpp>

namespace boost {
namespace buffers {

/** Metafunction to detect if a type is a read source.

    Data is obtained from a read source by calling @ref read one
    or more times with caller-provided buffers, until the function
    returns @ref error::eof.

    The members `size` and `rewind` are optional. The metafunctions
    @ref has_size and @ref has_rewind may be used to detect the
    availability of these capabilities.

    @code
    struct ReadSource
    {
        // (optional)
        std::uint64_t size() const noexcept;

        // (optional)
        void rewind();

        // returns error::eof when no more data
        template< class MutableBufferSequence >
        std::size_t
        read(
            MutableBufferSequence const& buffers,
            system::error_code& ec);
    };
    @endcode
*/
template<class T, class = void>
struct is_read_source
    : std::false_type
{
};

template<class T>
struct is_read_source<T, detail::void_t<typename
    std::enable_if<
        std::is_convertible<
            decltype(std::declval<T&>().read(
                std::declval<mutable_buffer>(),
                std::declval<system::error_code&>())),
            std::size_t>::value>::type>>
    : std::true_type
{
};

/** Metafunction to determine if `T::has_size` exists.
*/
template<class T, class = void>
struct has_size : std::false_type
{
};

template<class T>
struct has_size<T, detail::void_t<
    typename std::enable_if<
        std::is_convertible<
            decltype(std::declval<T const&>().size()),
            std::uint64_t>::value>::type>>
    : std::true_type
{
};

/** Metafunction to determin if `T::has_rewind` exists.
*/
template<class T, class = void>
struct has_rewind : std::false_type
{
};

template<class T>
struct has_rewind<T, detail::void_t<decltype(
    std::declval<T&>().rewind())>>
    : std::true_type
{
};

//-----------------------------------------------

/** Helper function for ReadSource implementations using single-buffer algorithms.

    This function assists models of the ReadSource concept by handling
    iteration over a mutable buffer sequence and invoking a user-provided
    function for each buffer. The function integrates the results and
    properly handles `error::eof`.

    The provided function `f` is invoked repeatedly, once for each
    buffer in the sequence, until all buffers are processed or an
    error occurs. When `f` returns `error::eof` and has read some
    bytes, that buffer's partial read is included in the total,
    but iteration stops and `error::eof` is not propagated to the
    caller. If `f` returns `error::eof` with zero bytes read on the
    first call, `error::eof` is returned immediately.

    @param buffers The mutable buffer sequence to read into.

    @param ec Set to the error, if any occurred.

    @param f A callable with signature `std::size_t(mutable_buffer, system::error_code&)`.
    The function should read data into the provided buffer and return the
    number of bytes read. On end-of-file, it should set `ec` to `error::eof`
    and return the number of bytes read (possibly zero).

    @return The total number of bytes read across all buffers.

    @tparam MutableBufferSequence A type meeting the requirements
    of MutableBufferSequence.

    @tparam Function A callable type compatible with the signature
    `std::size_t(mutable_buffer, system::error_code&)`.
*/
template<
    class MutableBufferSequence,
    class Function>
std::size_t
read_from_single_buffer(
    MutableBufferSequence const& buffers,
    system::error_code& ec,
    Function f)
{
    std::size_t total = 0;
    auto it = buffers::begin(buffers);
    auto const end_ = buffers::end(buffers);

    while(it != end_)
    {
        mutable_buffer mb = *it;
        if(mb.size() == 0)
        {
            ++it;
            continue;
        }

        system::error_code ec_tmp;
        auto const nread = f(mb, ec_tmp);
        total += nread;

        if(ec_tmp.failed())
        {
            if(ec_tmp == error::eof && total > 0)
            {
                // EOF after reading some bytes - return success
                ec = {};
                return total;
            }
            // Propagate the error
            ec = ec_tmp;
            return total;
        }

        // Successfully read from this buffer
        ++it;
    }

    ec = {};
    return total;
}

} // buffers
} // boost

#endif
