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
#include <atomic>
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

    @tparam IsConst If `true`, the sequence yields
    @ref const_buffer elements. If `false`, it yields
    @ref mutable_buffer elements.

    @see any_const_buffers, any_mutable_buffers
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

    /** Destructor.
    */
    ~any_buffers()
    {
        p_->destroy();
    }

    /** Constructor.
        Default-constructed objects are empty with zero length.
    */
    any_buffers() noexcept;

    /** Constructor.
    */
    any_buffers(
        any_buffers const& other) noexcept
    {
        other.p_->copy(*this);
    }

    /** Assignment.
    */
    any_buffers&
    operator=(
        any_buffers const& other) noexcept
    {
        if(this == &other)
            return *this;
        p_->destroy();
        other.p_->copy(*this);
        return *this;
    }

    /** Constructor.

        The type-erased buffer sequence is constructed
        from the specified buffer sequence, which must satisfy
        `ConstBufferSequence`. If `IsConst` is `false`, must
        also satisfy `MutableBufferSequence`.

        @param buffers The buffer sequence to type-erase.
    */
    template<class BufferSequence
        , class = typename std::enable_if<! std::is_same<
            any_buffers, typename std::decay<BufferSequence
                >::type>::value>::type>
    any_buffers(
        BufferSequence&& buffers)
    {
        using T = typename std::decay<BufferSequence>::type;
        construct(std::forward<BufferSequence>(buffers),
            std::integral_constant<bool, (
                sizeof(impl<T>) <= sbo_size)>{});
    }

    /** Return an iterator to the beginning.

        @return An iterator pointing to the first buffer,
        or `end()` if the sequence is empty.
    */
    const_iterator begin() const noexcept;

    /** Return an iterator to the end.

        @return An iterator pointing one past the last buffer.
    */
    const_iterator end() const noexcept;

private:
    friend struct any_buffers_test;

    static constexpr std::size_t sbo_size = 6 * sizeof(void*);

    static constexpr std::size_t iter_sbo_size = 4 * sizeof(void*);

    struct BOOST_SYMBOL_VISIBLE
        any_impl
    {
        virtual ~any_impl() = default;
        virtual bool is_small_buffers() const noexcept = 0;
        virtual bool is_small_iter() const noexcept = 0;
        virtual void destroy() const = 0;
        virtual void copy(any_buffers& dest) const = 0;
        virtual void it_copy(void*, void const*) const = 0;
        virtual void it_destroy(void*) const = 0;
        virtual void inc(void*) const = 0;
        virtual void dec(void*) const = 0;
        virtual auto deref(void const*) const -> value_type = 0;
        virtual bool equal(void const*, void const*) const = 0;
        virtual void begin(void*) const = 0;
        virtual void end(void*) const = 0;
    };

    template<class T, bool IsIterSmall = (sizeof(decltype(
        buffers::begin(std::declval<T const>()))) <= iter_sbo_size)>
    struct impl;

    // small buffer sequence
    template<class T>
    void construct(T&& t, std::true_type)
    {
        using U = typename std::decay<T>::type;
        p_ = ::new(&storage_) impl<U>(
            std::forward<T>(t));
    }

    template<class T>
    void construct(T&& t, std::false_type)
    {
        using U = typename std::decay<T>::type;
        p_ = new impl<U>(std::forward<T>(t));
    }

    bool is_small_buffers() const noexcept
    {
        return p_->is_small_buffers();
    }

    bool is_small_iter() const noexcept
    {
        return p_->is_small_iter();
    }

    alignas(std::max_align_t)
        unsigned char mutable storage_[sbo_size] = {};
    any_impl const* p_ = nullptr;
};

//-----------------------------------------------

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

// small iterator
template<bool IsConst>
template<class T, bool>
struct any_buffers<IsConst>::
    impl : any_impl
{
    using iter_t = decltype(buffers::begin(
        std::declval<T const&>()));

    template<class T_>
    explicit impl(T_&& t) noexcept
        : t_(std::forward<T_>(t))
    {
    }

    bool is_small_buffers() const noexcept override
    {
        return sizeof(*this) <= sbo_size;
    }

    bool is_small_iter() const noexcept override
    {
        return true;
    }

    void destroy() const override
    {
        destroy(std::integral_constant<bool,
            sizeof(*this) <= sbo_size>{});
    }

    void destroy(std::true_type) const // small buffers
    {
        this->~impl();
    }

    void destroy(std::false_type) const
    {
        if(--refs_ == 0)
            delete this;
    }

    void copy(any_buffers& dest) const override
    {
        copy(dest, std::integral_constant<bool,
            sizeof(*this) <= sbo_size>{});
    }

    void copy(any_buffers& dest, std::true_type) const // small buffers
    {
        dest.p_ = ::new(&dest.storage_) impl<T>(t_);
    }

    void copy(any_buffers& dest, std::false_type) const
    {
        ++refs_;
        dest.p_ = this;
    }

    void it_copy(void* dest, void const* src) const override
    {
        ::new(dest) iter_t(*static_cast<iter_t const*>(src));
    }

    void it_destroy(void* p) const override
    {
        static_cast<iter_t*>(p)->~iter_t();
    }

    void inc(void* p) const override
    {
        ++(*static_cast<iter_t*>(p));
    }

    void dec(void* p) const  override
    {
        --(*static_cast<iter_t*>(p));
    }

    value_type deref(void const* p) const override
    {
        return *(*static_cast<iter_t const*>(p));
    }

    bool equal(void const* it0, void const* it1) const override
    {
        return  *static_cast<iter_t const*>(it0) ==
                *static_cast<iter_t const*>(it1);
    }

    void begin(void* p) const override
    {
        ::new(p) iter_t(buffers::begin(t_));
    }

    void end(void* p) const override
    {
        ::new(p) iter_t(buffers::end(t_));
    }

private:
    T t_;
    std::atomic<std::size_t> mutable refs_{1};
};

template<bool IsConst>
template<class T>
struct any_buffers<IsConst>::
    impl<T, false> : any_impl
{
    struct iter_t
    {
        std::size_t i;
    };

    template<class T_>
    explicit impl(T_&& t) noexcept
        : t_(std::forward<T_>(t))
        , len_(length(t_))
    {
    }

    bool is_small_buffers() const noexcept override
    {
        return sizeof(*this) <=
            any_buffers<IsConst>::sbo_size;
    }

    bool is_small_iter() const noexcept override
    {
        return false;
    }

    void destroy() const override
    {
        destroy(std::integral_constant<bool,
            sizeof(*this) <= any_buffers<IsConst>::sbo_size>{});
    }

    void destroy(std::true_type) const // small buffers
    {
        this->~impl();
    }

    void destroy(std::false_type) const
    {
        if(--refs_ == 0)
            delete this;
    }

    void copy(any_buffers<IsConst>& dest) const override
    {
        copy(dest, std::integral_constant<bool,
            sizeof(*this) <= any_buffers<IsConst>::sbo_size>{});
    }

    void copy(any_buffers<IsConst>& dest,
        std::true_type) const // small buffers
    {
        dest.p_ = ::new(&dest.storage_) impl<T>(t_);
    }

    void copy(any_buffers<IsConst>& dest,
        std::false_type) const
    {
        ++refs_;
        dest.p_ = this;
    }

    void it_copy(void* dest, void const* src) const override
    {
        ::new(dest) iter_t(*static_cast<iter_t const*>(src));
    }

    void it_destroy(void* p) const override
    {
        static_cast<iter_t*>(p)->~iter_t();
    }

    void inc(void* p) const override
    {
        ++static_cast<iter_t*>(p)->i;
    }

    void dec(void* p) const  override
    {
        --static_cast<iter_t*>(p)->i;
    }

    typename any_buffers<IsConst>::value_type
    deref(void const* p) const override
    {
        auto const& it_ = *static_cast<iter_t const*>(p);
        auto it = buffers::begin(t_);
        for(auto i = it_.i; i; --i)
            ++it;
        return *it;
    }

    bool equal(void const* it0, void const* it1) const override
    {
        return  static_cast<iter_t const*>(it0)->i ==
                static_cast<iter_t const*>(it1)->i;
    }

    void begin(void* p) const override
    {
        ::new(p) iter_t{ 0 };
    }

    void end(void* p) const override
    {
        ::new(p) iter_t{ len_ };
    }

private:
    T t_;
    std::atomic<std::size_t> mutable refs_{1};
    std::size_t len_;
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
        p_->it_destroy(&storage_);
    }

    /** Default constructor.

        Constructs a singular iterator. A default-constructed
        iterator may only be assigned to or destroyed.
    */
    const_iterator() noexcept;

    /** Copy constructor.

        @param other The iterator to copy.
    */
    const_iterator(
        const_iterator const& other) noexcept
        : p_(other.p_)
    {
        p_->it_copy(&storage_, &other.storage_);
    }

    /** Copy assignment.

        @param other The iterator to copy.
        @return `*this`
    */
    const_iterator& operator=(
        const_iterator const& other) noexcept
    {
        if(this == &other)
            return *this;
        p_->it_destroy(&storage_);
        p_ = other.p_;
        p_->it_copy(&storage_, &other.storage_);
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
        if(p_ != other.p_)
            return false;
        return p_->equal(&storage_, &other.storage_);
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
        return p_->deref(&storage_);
    }

    /** Pre-increment.

        Advances the iterator to the next buffer.

        @return `*this`

        @pre The iterator is incrementable.
    */
    const_iterator&
    operator++() noexcept
    {
        p_->inc(&storage_);
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
        BOOST_ASSERT(p_ != nullptr);
        p_->dec(&storage_);
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
        any_impl const* p) noexcept
        : p_(p)
    {
        p_->begin(&storage_);
        BOOST_ASSERT(p_ != nullptr);
    }

    const_iterator(end_tag,
        any_impl const* p) noexcept
        : p_(p)
    {
        p_->end(&storage_);
        BOOST_ASSERT(p_ != nullptr);
    }

    alignas(std::max_align_t)
        unsigned char mutable storage_[iter_sbo_size] = {};
    any_buffers::any_impl const* p_ = nullptr;
};

//-----------------------------------------------

template<>
BOOST_BUFFERS_DECL
any_buffers<true>::
any_buffers() noexcept;

template<>
BOOST_BUFFERS_DECL
any_buffers<false>::
any_buffers() noexcept;

template<>
BOOST_BUFFERS_DECL
any_buffers<true>::
const_iterator::
const_iterator() noexcept;

template<>
BOOST_BUFFERS_DECL
any_buffers<false>::
const_iterator::
const_iterator() noexcept;

//-----------------------------------------------

template<bool IsConst>
auto
any_buffers<IsConst>::
begin() const noexcept ->
    const_iterator
{
    if(! p_)
        return const_iterator();
    return const_iterator(typename
        const_iterator::begin_tag{}, p_);
}

template<bool IsConst>
auto
any_buffers<IsConst>::
end() const noexcept ->
    const_iterator
{
    if(! p_)
        return const_iterator();
    return const_iterator(typename
        const_iterator::end_tag{}, p_);
}

} // buffers
} // boost

#endif