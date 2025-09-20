//
// Copyright (c) 2025 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/buffers
//

#ifndef BOOST_BUFFERS_ANY_HPP
#define BOOST_BUFFERS_ANY_HPP

#include <boost/buffers/detail/config.hpp>
#include <boost/buffers/buffer.hpp>
#include <boost/assert.hpp>
#include <iterator>
#include <new>
#include <type_traits>

namespace boost {
namespace buffers {

//------------------------------------------------

template<bool IsConst>
class any
{
public:
    class iterator;

    virtual ~any() = default;
    virtual iterator begin() const noexcept = 0;
    virtual iterator end() const noexcept = 0;
};

using any_const_buffers = any<true>;
using any_mutable_buffers = any<false>;

//------------------------------------------------

template<bool IsConst>
class any_ref
{
    any<IsConst> const& bs_;

public:
    using iterator = typename any<IsConst>::iterator;

    any_ref(any<IsConst> const& bs) noexcept
        : bs_(bs)
    {
    }

    iterator begin() const noexcept
    {
        return bs_.begin();
    }

    iterator end() const noexcept
    {
        return bs_.end();
    }
};

using any_const_ref = any_ref<true>;
using any_mutable_ref = any_ref<false>;

//------------------------------------------------

template<
    class BufferRange,
    bool IsConst = is_const_buffer_sequence<BufferRange>::value>
class any_impl : public any<IsConst>
{
    BufferRange b_;

public:
    using iterator = typename any<IsConst>::iterator;

    explicit
    any_impl(
        BufferRange const& b)
        : b_(b)
    {
    }

    iterator begin() const noexcept override
    {
        return iterator(buffers::begin(b_));
    }

    iterator end() const noexcept override
    {
        return iterator(buffers::end(b_));
    }
};

//------------------------------------------------

template<bool IsConst>
class any<IsConst>::iterator
{
public:
    static constexpr std::size_t Bytes = 64;
    using value_type = typename std::conditional<
        IsConst, const_buffer, mutable_buffer>::type;
    using reference = value_type;
    using pointer = void;
    using difference_type = std::ptrdiff_t;
    using iterator_category =
        std::bidirectional_iterator_tag;
#if defined(__cpp_concepts) || defined(__cpp_lib_concepts)
    using iterator_concept = std::bidirectional_iterator_tag; // (since C++20)
#endif

    ~iterator()
    {
        get().destroy();
    }

    iterator() noexcept
    {
        ::new(buf_) base;
    }

    iterator(
        iterator const& other) noexcept
    {
        other.get().copy(buf_);
    }

    iterator&
        operator=(iterator const& other) noexcept
    {
        other.get().assign(buf_);
        return *this;
    }

    template<class It>
    iterator(It const& it) noexcept
    {
        ::new(buf_) impl<It>(it);
    }

    bool
    operator==(iterator const& other) const noexcept
    {
        return get().equal(other.buf_);
    }

    bool
    operator!=(
        iterator const& other) const noexcept
    {
        return !(get().equal(other.buf_));
    }

    reference
    operator*() const noexcept
    {
        return get().deref();
    }

    iterator&
    operator++() noexcept
    {
        get().increment();
        return *this;
    }

    iterator
    operator++(int) noexcept
    {
        auto temp = *this;
        get().increment();
        return temp;
    }

    iterator&
    operator--() noexcept
    {
        get().decrement();
        return *this;
    }

    iterator
    operator--(int) noexcept
    {
        auto temp = *this;
        get().decrement();
        return temp;
    }

private:
    // this is also a default-constructed iterator
    struct base
    {
        base() noexcept {}
        virtual ~base() {}
        virtual void destroy() const noexcept {}
        virtual void copy(void*) const noexcept {}
        virtual void assign(void*) const noexcept {}
        virtual bool equal(void const*) const noexcept { return true; }
        virtual reference deref() const noexcept { return {}; }
        virtual void increment() noexcept {}
        virtual void decrement() noexcept {}
    };

    template<class It>
    struct impl;

    base& get() noexcept
    {
        return *reinterpret_cast<base*>(buf_);
    }

    base const& get() const noexcept
    {
        return *reinterpret_cast<base const*>(buf_);
    }

    char buf_[Bytes];
};

//------------------------------------------------

template<bool IsConst>
template<class It>
struct any<IsConst>::iterator::impl : base
{
    It it_{};

    impl() noexcept
    {
    }

    impl(It const& it) noexcept
        : it_(it)
    {
    }

    void destroy() const noexcept override
    {
        this->~impl();
    }

    void copy(void* p) const noexcept override
    {
        ::new(p) impl(*this);
    }

    void assign(void* p) const noexcept override
    {
        reinterpret_cast<impl*>(p)->it_ = it_;
    }

    bool equal(void const* p) const noexcept override
    {
        return reinterpret_cast<impl const*>(p)->it_ == it_;
    }

    reference deref() const noexcept override
    {
        return *it_;
    }

    void increment() noexcept override
    {
        ++it_;
    }

    void decrement() noexcept override
    {
        --it_;
    }
};

//------------------------------------------------

template<class BufferRange>
auto make_any(
    BufferRange const& b) ->
    any_impl<BufferRange>
{
    return any_impl<BufferRange>(b);
}

} // buffers
} // boost

#endif
