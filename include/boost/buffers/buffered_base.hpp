//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/http_proto
//

#ifndef BOOST_BUFFERS_BUFFERED_BASE_HPP
#define BOOST_BUFFERS_BUFFERED_BASE_HPP

#include <boost/buffers/detail/config.hpp>
#include <cstddef>

namespace boost {
namespace buffers {

/** Base class for buffered algorithms

    Algorithms implementing @ref filter,
    @ref source, or @ref sink inherit from
    this common interface.
*/
class BOOST_SYMBOL_VISIBLE
    buffered_base
{
    bool inited_ = false;

public:
    /** Provides memory to buffered algorithms.
    */
    class allocator;

    /** Destructor.
    */
    BOOST_BUFFERS_DECL
    virtual
    ~buffered_base() = 0;

    /** Return true if init was called.
    */
    bool
    is_inited() const noexcept
    {
        return inited_;
    }

    /** Called to initialize the algorithm.

        This virtual function is called once
        before any other members. The default
        implementation does nothing. The purpose
        of this function is to give the algorithm
        the opportunity to allocate memory using
        the specified allocator, which could
        offer advantages.
        <br>
        Subclasses are still required to operate
        correctly even when insufficient memory
        is available from the allocator. In this
        case they should simply allocate normally.

        @throws std::logic_error Function called more than once

        @param a The allocator which may be used
            by the algorithm to obtain contiguous
            storage.
    */
    BOOST_BUFFERS_DECL
    void
    init(allocator& a);

    /** Called to initialize the algorithm.

        This function will call @ref init, and
        for the duration of the invocation the
        maximum size returned by the allocator
        will be initially set to `max_size`.
        When the function returns, the maximum
        size of the allocator will be restored
        to its previous value, minus the amount
        used in any calls to allocate.
        <br>
        This function is designed to be used by
        implementations which need to initialize
        sub-objects which implement the
        @ref buffered_base interface.
        
        @param a The allocator which may be used
            by the algorithm to obtain contiguous
            storage.

        @param max_size The largest allowed
            total amount of bytes passed to the
            allocator's allocate function.
    */
    BOOST_BUFFERS_DECL
    void
    init(
        allocator& a,
        std::size_t max_size);

#ifdef BOOST_BUFFERS_DOCS
protected:
#else
private:
#endif
    BOOST_BUFFERS_DECL
    virtual
    void
    do_init(allocator& a);
};

//------------------------------------------------

/** Provides memory to buffered algorithms.
*/
class buffered_base::allocator
{
public:
    /** Constructor

        Default constructed objects return
        zero from @ref max_size.
    */
    allocator() = default;

    /** Constructor

        This function constructs an allocator
        which uses the specified contiguous
        buffer. Calls to allocate will return
        parcels of the buffer from either the
        beginning or the end depending on the
        value of `downwards`.

        @par Preconditions
        @code
        p != nullptr || n == 0
        @endcode

        @par Exception Safety
        Throws nothing.

        @param p A pointer to contiguous storage.
            This may be `nullptr` if `n == 0`.

        @param n The number of valid bytes of
            storage pointed to by p. This may
            be zero.

        @param downwards When true, calls to
            allocate will return storage from
            the end of the memory pointed to
            by `p` rather than the beginning.
    */
    allocator(
        void* p,
        std::size_t n,
        bool downwards) noexcept
        : base_(static_cast<
            unsigned char*>(p))
        , size_(n)
        , down_(downwards)
    {
    }

    /** The maximum amount that can be successfully returned from reserve
    */
    std::size_t
    max_size() const noexcept
    {
        return size_;
    }

    /** Return the total number of bytes allocated
    */
    std::size_t
    size_used() const noexcept
    {
        return size_used_;
    }

    /** Return a pointer to at least n bytes of contiguous storage

        Allocated storage will be automatically
        deallocated when the @ref filter,
        @ref sink, or @ref source is destroyed.

        @throws std::invalid_argument `n > max_size()`

        @return A pointer to uninitialized storage.

        @param n The number of bytes.
    */
    BOOST_BUFFERS_DECL
    void*
    allocate(std::size_t n);

private:
    void
    remove(
        std::size_t n) noexcept
    {
        if(down_)
            base_ += n;
        size_ -= n;
    }

    void
    restore(
        std::size_t n) noexcept
    {
        if(down_)
            base_ -= n;
        size_ += n;
    }

    friend class buffered_base;

    unsigned char* base_ = nullptr;
    std::size_t size_ = 0;
    std::size_t size_used_ = 0;
    bool down_ = false;
};

} // buffers
} // boost

#endif
