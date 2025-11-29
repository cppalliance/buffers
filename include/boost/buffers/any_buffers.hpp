//
// Copyright (c) 2025 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/buffers
//

#ifndef BOOST_BUFFERS_ANY_BUFFERS_HPP
#define BOOST_BUFFERS_ANY_BUFFERS_HPP

#include <boost/buffers/detail/config.hpp>
#include <boost/buffers/buffer.hpp>
#include <boost/core/detail/static_assert.hpp>
#include <boost/assert.hpp>

#include <cstddef>
#include <new>
#include <type_traits>

namespace boost {
namespace buffers {

/** A type-erased buffer sequence.

    This class template wraps any buffer sequence and
    exposes it through a uniform interface, hiding the
    concrete type. Iteration is performed via a type-erased
    bidirectional iterator.

    The implementation uses small buffer optimization (SBO)
    for iterators that are small, trivially aligned, and
    nothrow copy constructible. Larger iterators fall back
    to an index-based traversal strategy.

    @note The wrapped buffer sequence must remain valid
    for the lifetime of this object.

    @tparam IsConst If `true`, the sequence yields
    @ref const_buffer elements. If `false`, it yields
    @ref mutable_buffer elements.

    @see any_const_buffers, any_mutable_buffers, make_any_buffers
*/
template<bool IsConst>
class any_buffers
{
public:
    /** The buffer type returned when dereferencing iterators.

        This is @ref const_buffer when `IsConst` is `true`,
        otherwise @ref mutable_buffer.
    */
    using value_type = typename std::conditional<
        IsConst, const_buffer, mutable_buffer>::type;

    /** A bidirectional iterator over the buffer sequence.

        @see begin, end
    */
    class const_iterator;

    /** Return an iterator to the beginning.

        @return An iterator pointing to the first buffer,
        or `end()` if the sequence is empty.
    */
    const_iterator begin() const noexcept;

    /** Return an iterator to the end.

        @return An iterator pointing one past the last buffer.
    */
    const_iterator end() const noexcept;

    /** Construct from a buffer sequence.

        Wraps the given buffer sequence for type-erased
        access. The caller must ensure `bs` remains valid
        for the lifetime of this object.

        @param bs The buffer sequence to wrap. Must satisfy
        `ConstBufferSequence`. If `IsConst` is `false`, must
        also satisfy `MutableBufferSequence`.

        @tparam BufferSequence The concrete buffer sequence type.
    */
    template<class BufferSequence>
    any_buffers(BufferSequence const& bs);

private:
    static constexpr std::size_t sbo_size = 4 * sizeof(void*);

    struct iter_ops;

    template<class Buffers>
    static iter_ops const* make_ops()
    {
        using iter_t = decltype(buffers::begin(
            std::declval<Buffers const&>()));
        static constexpr bool is_small =
            sizeof(iter_t) <= sbo_size &&
            alignof(iter_t) <= alignof(std::max_align_t) &&
            std::is_nothrow_copy_constructible<iter_t>::value;
        return make_ops<Buffers>(
            std::integral_constant<bool, is_small>{});
    }

    template<class Buffers>
    static iter_ops const* make_ops(std::true_type);

    template<class Buffers>
    static iter_ops const* make_ops(std::false_type);

    template<class Buffers>
    std::size_t get_length(Buffers const& bs)
    {
        using iter_t = decltype(buffers::begin(
            std::declval<Buffers const&>()));
        static constexpr bool is_small =
            sizeof(iter_t) <= sbo_size &&
            alignof(iter_t) <= alignof(std::max_align_t) &&
            std::is_nothrow_copy_constructible<iter_t>::value;
        return get_length(bs, std::integral_constant<bool, is_small>{});
    }

    template<class Buffers>
    std::size_t get_length(Buffers const&, std::true_type)
    {
        // no-op since length is not needed
        return 0;
    }

    template<class Buffers>
    std::size_t get_length(Buffers const& bs, std::false_type)
    {
        return buffers::length(bs);
    }

    void const* bs_ = nullptr;
    std::size_t len_ = 0;
    iter_ops const* ops_ = nullptr;
};

/** Alias for a type-erased const buffer sequence.

    Equivalent to `any_buffers<true>`.

    @see any_buffers, any_mutable_buffers
*/
using any_const_buffers = any_buffers<true>;

/** Alias for a type-erased mutable buffer sequence.

    Equivalent to `any_buffers<false>`.

    @see any_buffers, any_const_buffers
*/
using any_mutable_buffers = any_buffers<false>;

//-----------------------------------------------

template<bool IsConst>
struct any_buffers<IsConst>::
    iter_ops
{
    void (*copy)(void*, void const*);
    void (*destroy)(void*);
    void (*increment)(void*);
    void (*decrement)(void*);
    value_type (*deref)(void const*);
    bool (*equal)(void const*, void const*);
    void (*construct_begin)(void*, void const*);
    void (*construct_end)(void*, void const*, std::size_t);
};

//-----------------------------------------------

/** A bidirectional iterator for @ref any_buffers.

    This iterator provides type-erased access to the
    underlying buffer sequence elements. It models
    `BidirectionalIterator` and returns buffer objects
    by value.
*/
template<bool IsConst>
class any_buffers<IsConst>::
    const_iterator
{
public:
    /** The buffer type returned by dereferencing.
    */
    using value_type = typename any_buffers::value_type;

    /** The type returned by `operator*`.

        Buffers are returned by value.
    */
    using reference = value_type;

    /** Pointer type (void, not used).
    */
    using pointer = void;

    /** Signed integer type for iterator differences.
    */
    using difference_type = std::ptrdiff_t;

    /** Iterator category tag.
    */
    using iterator_category =
        std::bidirectional_iterator_tag;

#if defined(__cpp_concepts) || defined(__cpp_lib_concepts)
    /** Iterator concept tag (C++20).
    */
    using iterator_concept = std::bidirectional_iterator_tag;
#endif

    /** Destructor.

        Destroys the type-erased iterator state.
    */
    ~const_iterator()
    {
        ops_->destroy(&storage_);
    }

    /** Default constructor.

        Constructs a singular iterator. A default-constructed
        iterator may only be assigned to or destroyed.
    */
    const_iterator() = default;

    /** Copy constructor.

        @param other The iterator to copy.
    */
    const_iterator(
        const_iterator const& other) noexcept
        : ops_(other.ops_)
    {
        ops_->copy(&storage_, &other.storage_);
    }

    /** Copy assignment.

        @param other The iterator to copy.
        @return `*this`
    */
    const_iterator& operator=(
        const_iterator const& other) noexcept
    {
        if(this != &other)
        {
            ops_->destroy(&storage_);

            ops_ = other.ops_;
            ops_->copy(&storage_, &other.storage_);
        }
        return *this;
    }

    /** Test for equality.

        @param other The iterator to compare.
        @return `true` if both iterators point to the
        same element of the same sequence.
    */
    bool
    operator==(
        const_iterator const& other) const noexcept
    {
        if(ops_ != other.ops_)
            return false;
        if(ops_ == nullptr)
            return false;
        return ops_->equal(&storage_, &other.storage_);
    }

    /** Test for inequality.

        @param other The iterator to compare.
        @return `true` if the iterators point to
        different elements or different sequences.
    */
    bool
    operator!=(
        const_iterator const& other) const noexcept
    {
        return !(*this == other);
    }

    /** Dereference the iterator.

        @return The buffer at the current position.

        @pre The iterator is dereferenceable
        (not default-constructed or past-the-end).
    */
    reference
    operator*() const noexcept
    {
        BOOST_ASSERT(ops_ != nullptr);
        return ops_->deref(&storage_);
    }

    /** Pre-increment.

        Advances the iterator to the next buffer.

        @return `*this`

        @pre The iterator is incrementable.
    */
    const_iterator&
    operator++() noexcept
    {
        BOOST_ASSERT(ops_ != nullptr);
        ops_->increment(&storage_);
        return *this;
    }

    /** Post-increment.

        Advances the iterator to the next buffer.

        @return A copy of the iterator before incrementing.

        @pre The iterator is incrementable.
    */
    const_iterator
    operator++(int) noexcept
    {
        auto temp = *this;
        ++(*this);
        return temp;
    }

    /** Pre-decrement.

        Moves the iterator to the previous buffer.

        @return `*this`

        @pre The iterator is decrementable.
    */
    const_iterator&
    operator--() noexcept
    {
        BOOST_ASSERT(ops_ != nullptr);
        ops_->decrement(&storage_);
        return *this;
    }

    /** Post-decrement.

        Moves the iterator to the previous buffer.

        @return A copy of the iterator before decrementing.

        @pre The iterator is decrementable.
    */
    const_iterator
    operator--(int) noexcept
    {
        auto temp = *this;
        --(*this);
        return temp;
    }

private:
    friend class any_buffers;

    struct begin_tag {};
    struct end_tag {};

    const_iterator(begin_tag,
        iter_ops const* ops,
        void const* bs) noexcept
        : ops_(ops)
    {
        ops_->construct_begin(&storage_, bs);
    }

    const_iterator(end_tag,
        iter_ops const* ops,
        void const* bs,
        std::size_t len) noexcept
        : ops_(ops)
    {
        ops_->construct_end(&storage_, bs, len);
    }

    alignas(std::max_align_t)
        unsigned char mutable storage_[sbo_size] = {};
    any_buffers::iter_ops const* ops_ = nullptr;
};

//-----------------------------------------------

namespace detail {

template<class T>
class holder
{
protected:
    template<class T_>
    explicit holder(T_&& t) noexcept
        : t_(std::forward<T_>(t))
    {
    }

    T t_;
};

} // detail

/** An owning type-erased buffer sequence.

    This class stores a copy of a buffer sequence and
    provides type-erased access to it. Unlike @ref any_buffers,
    this class owns the underlying buffer sequence.

    @tparam BufferSequence The concrete buffer sequence type.

    @tparam IsConst If `true`, exposes @ref const_buffer
    elements. Defaults to `true` if `BufferSequence` is
    not a mutable buffer sequence.

    @see any_buffers, make_any_buffers
*/
template<class BufferSequence,
    bool IsConst = ! is_mutable_buffer_sequence<BufferSequence>::value>
class any_buffers_impl
    : private detail::holder<BufferSequence>
    , public any_buffers<IsConst>
{
public:
    /** Move constructor.
    */
    any_buffers_impl(any_buffers_impl&&) = default;

    /** Construct from a buffer sequence.

        Stores a copy of `bs` and initializes the
        type-erased view.

        @param bs The buffer sequence to store.

        @tparam BufferSequence_ The source type,
        which must be convertible to `BufferSequence`.
    */
    template<class BufferSequence_>
    any_buffers_impl(
        BufferSequence_&& bs) noexcept
        : detail::holder<BufferSequence>(
            std::forward<BufferSequence_>(bs))
        , any_buffers<IsConst>(this->t_)
    {
    }
};

/** Create an owning type-erased buffer sequence.

    Returns an @ref any_buffers_impl that stores a
    decayed copy of `bs` and provides type-erased
    access to it.

    @param bs The buffer sequence to wrap.

    @return An @ref any_buffers_impl owning a copy of `bs`.

    @tparam BufferSequence The buffer sequence type.

    @see any_buffers, any_buffers_impl
*/
template<class BufferSequence>
auto
make_any_buffers(
    BufferSequence&& bs) ->
        any_buffers_impl<typename
            std::decay<BufferSequence>::type>
{
    return any_buffers_impl<typename
        std::decay<BufferSequence>::type>(
            std::forward<BufferSequence>(bs));
}

//-----------------------------------------------

template<bool IsConst>
template<class BufferSequence>
any_buffers<IsConst>::
any_buffers(
    BufferSequence const& bs)
    : bs_(&bs)
    , len_(get_length(bs))
    , ops_(make_ops<BufferSequence>())
{
    BOOST_CORE_STATIC_ASSERT(
        is_const_buffer_sequence<BufferSequence>::value);
    BOOST_CORE_STATIC_ASSERT(IsConst ||
        is_mutable_buffer_sequence<BufferSequence>::value);
}

// small iterators
template<bool IsConst>
template<class Buffers>
auto
any_buffers<IsConst>::
make_ops(std::true_type) ->
    iter_ops const*
{
    using iter_t = decltype(buffers::begin(
        std::declval<Buffers const&>()));
    static const iter_ops ops = {
        // copy
        [](void* dest, void const* src)
        {
            ::new(dest) iter_t(*static_cast<iter_t const*>(src));
        },
        // destroy
        [](void* p)
        {
            static_cast<iter_t*>(p)->~iter_t();
        },
        // increment
        [](void* p)
        {
            ++(*static_cast<iter_t*>(p));
        },
        // decrement
        [](void* p)
        {
            --(*static_cast<iter_t*>(p));
        },
        // deref
        [](void const* p) -> value_type
        {
            return *(*static_cast<iter_t const*>(p));
        },
        // equal
        [](void const* a, void const* b) -> bool
        {
            return  *static_cast<iter_t const*>(a) ==
                    *static_cast<iter_t const*>(b);
        },
        // construct_begin
        [](void* storage, void const* bs)
        {
            ::new(storage) iter_t(buffers::begin(
                *static_cast<Buffers const*>(bs)));
        },
        // construct_end
        [](void* storage, void const* bs, std::size_t)
        {
            ::new(storage) iter_t(buffers::end(
                *static_cast<Buffers const*>(bs)));
        }
    };
    return &ops;
}

// large iterators
template<bool IsConst>
template<class Buffers>
auto
any_buffers<IsConst>::
make_ops(std::false_type) ->
    iter_ops const*
{
    struct iter_t
    {
        std::size_t i_;
        Buffers const* bs_;
    };

    BOOST_CORE_STATIC_ASSERT(sizeof(iter_t) <= sbo_size);

    static const iter_ops ops = {
        // copy
        [](void* dest, void const* src)
        {
            ::new(dest) iter_t(*static_cast<iter_t const*>(src));
        },
        // destroy
        [](void* p)
        {
            static_cast<iter_t*>(p)->~iter_t();
        },
        // increment
        [](void* p)
        {
            ++static_cast<iter_t*>(p)->i_;
        },
        // decrement
        [](void* p)
        {
            --static_cast<iter_t*>(p)->i_;
        },
        // deref
        [](void const* p) -> value_type
        {
            auto const& it_ = *static_cast<iter_t const*>(p);
            auto it = buffers::begin(*it_.bs_);
            for(auto i = it_.i_; i; --i)
                ++it;
            return *it;
        },
        // equal
        [](void const* a, void const* b) -> bool
        {
            auto const& it0 = *static_cast<iter_t const*>(a);
            auto const& it1 = *static_cast<iter_t const*>(b);
            return
                it0.i_  == it1.i_ &&
                it0.bs_ == it1.bs_;   
        },
        // construct_begin
        [](void* storage, void const* bs)
        {
            ::new(storage) iter_t{ 0,
                static_cast<Buffers const*>(bs) };
        },
        // construct_end
        [](void* storage, void const* bs, std::size_t len)
        {
            ::new(storage) iter_t{ len,
                static_cast<Buffers const*>(bs) };
        }
    };
    return &ops;
}

//-----------------------------------------------

template<bool IsConst>
auto
any_buffers<IsConst>::
begin() const noexcept ->
    const_iterator
{
    return const_iterator(
        typename const_iterator::begin_tag{},
        ops_, bs_);
}

template<bool IsConst>
auto
any_buffers<IsConst>::
end() const noexcept ->
    const_iterator
{
    return const_iterator(
        typename const_iterator::end_tag{},
        ops_, bs_, len_);
}

//-----------------------------------------------

} // buffers
} // boost

#endif