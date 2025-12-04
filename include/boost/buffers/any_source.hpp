//
// Copyright (c) 2025 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/beast2
//

#ifndef BOOST_BUFFERS_ANY_SOURCE_HPP
#define BOOST_BUFFERS_ANY_SOURCE_HPP

#include <boost/buffers/detail/config.hpp>
#include <boost/buffers/any_buffers.hpp>
#include <boost/buffers/buffer.hpp>
#include <boost/buffers/copy.hpp>
#include <boost/buffers/data_source.hpp>
#include <boost/buffers/error.hpp>
#include <boost/buffers/read_source.hpp>
#include <boost/buffers/slice.hpp>
#include <boost/buffers/detail/except.hpp>

namespace boost {
namespace buffers {

/** A type erased source.

    An object of this type represents shared ownership of a type-erased read\
    source or data source.
    It provides a uniform interface for reading the source data regardless of
    how the source is implemented. Accessing the bytes is achieved by calling
    @ref read which reads data into a caller-provided buffer. Alternatively,
    when @ref has_buffers returns `true` the source consists of buffers in memory,
    and they can be accessed directly by calling @ref get_buffers.

    Example sources include:
    - in-memory buffers
    - streaming file data
    - generated data

    @note @ref any_source is copyable, and the copies share ownership of the
    underlying source. Changes to the state of one copy, such as reading
    or rewinding, will be visible in all copies.

    Type-erased sources can always be rewound to the beginning by
    calling @ref rewind. Therefore, a source can be read multiple times.

    @par Thread Safety
    Unsafe.
*/
class any_source
{
public:
    /** Constructor

        Default-constructed sources are empty.
    */
    BOOST_BUFFERS_DECL
    any_source() noexcept;

    any_source(any_source const&) = default;

    any_source& operator=(any_source const&) = default;

    /** Construct a read source.
    */
    template<class ReadSource, typename std::enable_if<
        std::conditional<
            std::is_same<typename std::decay<
                ReadSource>::type, any_source>::value,
            std::false_type,
            is_read_source<typename std::decay<ReadSource>::type>
        >::type::value, int>::type = 0>
    any_source(ReadSource&& source);

    /** Construct a read source with a known size.
    */
    template<class ReadSource, typename std::enable_if<
        std::conditional<
            std::is_same<typename std::decay<
                ReadSource>::type, any_source>::value,
            std::false_type,
            is_read_source<typename std::decay<ReadSource>::type>
        >::type::value, int>::type = 0>
    any_source(
        std::size_t known_size,
        ReadSource&& source);

    /** Construct a data source.
    */
    template<class DataSource, typename std::enable_if<
        std::conditional<
            std::is_same<typename std::decay<
                DataSource>::type, any_source>::value,
            std::false_type,
            is_data_source<typename std::decay<DataSource>::type>
        >::type::value, int>::type = 0>
    any_source(DataSource&& source);

    /** Return `true` if the size of the source is known.
    */
    bool has_size() const noexcept
    {
        return sp_->has_size();
    }

    /** Return `true` if the source consists of buffers in memory.
        When the source consists of buffers in memory, they can
        also be accessed directly using @ref get_buffers.
    */
    bool has_buffers() const noexcept
    {
        return sp_->has_buffers();
    }

    /** Return the size of the source, if available.
        @throw std::invalid_argument if @ref has_size returns `false`.
        @return The size of the source in bytes.
    */
    auto size() const -> std::size_t
    {
        return sp_->size();
    }

    /** Return the buffers representing the source, if available.
        @throw std::invalid_argument if @ref has_buffers returns `false`.
        @return A buffer sequence representing the source.
    */
    auto data() const ->
        any_const_buffers
    {
        return sp_->data();
    }

    /** Rewind the source to the beginning.
        This allows the source to be accessed from the start when calling @read.
    */
    void rewind()
    {
        sp_->rewind();
    }

    /** Read from the source into a caller-provided buffer.

        When the last byte of data has been read,
        @p ec is set to @ref error::eof.

        @param dest A pointer to the buffer to read into.
        @param n The maximum number of bytes to read.
        @param ec Set to the error, if any occurred.
        @return The number of bytes read, which may be
        less than the number requested. 
    */
    auto
    read(void* dest, std::size_t n,
        system::error_code& ec) ->
            std::size_t
    {
        return sp_->read(dest, n, ec);
    }

private:
    struct BOOST_BUFFERS_DECL
        any_impl
    {
        virtual ~any_impl() = 0;
        virtual bool has_size() const noexcept;
        virtual bool has_buffers() const noexcept;
        virtual std::size_t size() const;
        virtual auto data() const -> any_const_buffers;
        virtual void rewind() = 0;
        virtual std::size_t read(
            void* dest, std::size_t n,
            system::error_code& ec) = 0;
    };

    std::shared_ptr<any_impl> sp_;
};

//-----------------------------------------------

template<class ReadSource, typename std::enable_if<
    std::conditional<
        std::is_same<typename std::decay<
            ReadSource>::type, any_source>::value,
        std::false_type,
        is_read_source<typename std::decay<ReadSource>::type>
    >::type::value, int>::type>
any_source::
any_source(
    ReadSource&& source)
{
    struct model : any_impl
    {
        system::error_code ec_;
        typename std::decay<ReadSource>::type source_;

        explicit model(ReadSource&& source)
            : source_(std::forward<ReadSource>(source))
        {
        }

        void rewind() override
        {
            ec_ = {};
            source_.rewind();
        }

        std::size_t read(
            void* dest,
            std::size_t size,
            system::error_code& ec) override
        {
            if(ec_.failed())
            {
                ec = ec_;
                return 0;
            }
            auto nread = source_.read(
                mutable_buffer(dest, size), ec);
            ec_ = ec;
            return nread;
        }
    };

    sp_ = std::make_shared<model>(
        std::forward<ReadSource>(source));
}

/** Construct a streaming source source with a known size.
*/
template<class ReadSource, typename std::enable_if<
    std::conditional<
        std::is_same<typename std::decay<
            ReadSource>::type, any_source>::value,
        std::false_type,
        is_read_source<typename std::decay<ReadSource>::type>
    >::type::value, int>::type>
any_source::
any_source(
    std::size_t known_size,
    ReadSource&& source)
{
    struct model : any_impl
    {
        std::size_t size_;
        system::error_code ec_;
        typename std::decay<ReadSource>::type source_;

        model(
            ReadSource&& source,
            std::size_t known_size)
            : size_(known_size)
            , source_(std::forward<ReadSource>(source))
        {
        }

        bool has_size() const noexcept override
        {
            return true;
        }

        std::size_t size() const override
        {
            return size_;
        }

        void rewind() override
        {
            ec_ = {};
            source_.rewind();
        }

        std::size_t read(
            void* dest,
            std::size_t size,
            system::error_code& ec) override
        {
            if(ec_.failed())
            {
                ec = ec_;
                return 0;
            }
            auto nread = source_.read(
                mutable_buffer(dest, size), ec);
            ec_ = ec;
            return nread;
        }
    };

    sp_ = std::make_shared<model>(
        std::forward<ReadSource>(source), known_size);
}

/** Construct a buffers source source.
*/
template<class DataSource, typename std::enable_if<
    std::conditional<
        std::is_same<typename std::decay<
            DataSource>::type, any_source>::value,
        std::false_type,
        is_data_source<typename std::decay<DataSource>::type>
    >::type::value, int>::type>
any_source::
any_source(
    DataSource&& source)
{
    struct model : any_impl
    {
        typename std::decay<DataSource>::type source_;
        std::size_t size_ = 0;
        std::size_t nread_ = 0;

        explicit model(
            DataSource&& source) noexcept
            : source_(std::forward<DataSource>(source))
            , size_(buffers::size(source_.data()))
        {
        }

        bool has_size() const noexcept override
        {
            return true;
        }

        bool has_buffers() const noexcept override
        {
            return true;
        }

        std::size_t size() const override
        {
            return size_;
        }

        any_const_buffers
        data() const override
        {
            return source_.data();
        }

        void rewind() override
        {
            nread_ = 0;
        }

        std::size_t read(
            void* dest,
            std::size_t n0,
            system::error_code& ec) override
        {
            std::size_t n = copy(
                mutable_buffer(dest, n0),
                sans_prefix(source_.data(), nread_));
            nread_ += n;
            if(nread_ >= size_)
                ec = error::eof;
            else
                ec = {};
            return n;
        }
    };

    // VFALCO this requires DataSource to be nothrow
    // move constructible for strong exception safety.
    sp_ = std::make_shared<model>(
        std::forward<DataSource>(source));
}

} // buffers
} // boost

#endif
