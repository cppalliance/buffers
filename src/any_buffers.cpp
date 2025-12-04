//
// Copyright (c) 2025 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/buffers
//

#include <boost/buffers/any_buffers.hpp>
#include <boost/buffers/detail/except.hpp>

namespace boost {
namespace buffers {

template<>
any_buffers<true>::
any_buffers() noexcept
{
    struct impl : any_impl
    {
        bool is_small_buffers() const noexcept override
        {
            return true;
        }

        bool is_small_iter() const noexcept override
        {
            return true;
        }

        void destroy() const override
        {
        }

        void copy(any_buffers& dest) const override
        {
            dest.p_ = this;
        }

        void it_copy(void*, void const*) const override
        {
        }

        void it_destroy(void*) const override
        {
        }

        void inc(void*) const override
        {
        }

        void dec(void*) const override
        {
        }

        value_type deref(void const*) const override
        {
            return {};
        }

        bool equal(void const*, void const*) const override
        {
            return true;
        }

        void begin(void*) const override
        {
        }

        void end(void*) const override
        {
        }
    };

    static impl const instance;
    p_ = &instance;
}

template<>
any_buffers<false>::
any_buffers() noexcept
{
    struct impl : any_impl
    {
        bool is_small_buffers() const noexcept override
        {
            return true;
        }

        bool is_small_iter() const noexcept override
        {
            return true;
        }

        void destroy() const override
        {
        }

        void copy(any_buffers& dest) const override
        {
            dest.p_ = this;
        }

        void it_copy(void*, void const*) const override
        {
        }

        void it_destroy(void*) const override
        {
        }

        void inc(void*) const override
        {
        }

        void dec(void*) const override
        {
        }

        value_type deref(void const*) const override
        {
            return {};
        }

        bool equal(void const*, void const*) const override
        {
            return true;
        }

        void begin(void*) const override
        {
        }

        void end(void*) const override
        {
        }
    };

    static impl const instance;
    p_ = &instance;
}

template<>
any_buffers<true>::
any_buffers::
const_iterator::
const_iterator() noexcept
    : p_(any_buffers<true>().begin().p_)
{
}

template<>
any_buffers<false>::
any_buffers::
const_iterator::
const_iterator() noexcept
    : p_(any_buffers<false>().begin().p_)
{
}

} // buffers
} // boost
