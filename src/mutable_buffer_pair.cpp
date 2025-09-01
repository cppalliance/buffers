//
// Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/buffers
//

#include <boost/buffers/mutable_buffer_pair.hpp>
#include <boost/buffers/sans_prefix.hpp>
#include <boost/buffers/slice.hpp>

namespace boost {
namespace buffers {

mutable_buffer_pair
mutable_buffer_pair::
prefix_impl(
    std::size_t n) const noexcept
{
    auto const it0 = begin();
    if(n <= it0->size())
        return { {
            it0->data(), n },
            mutable_buffer{} };
    n -= it0->size();
    auto it1 = it0;
    ++it1;
    if(n < it1->size())
        return { *it0, {
            it1->data(), n } };
    return *this;
}

mutable_buffer_pair
mutable_buffer_pair::
suffix_impl(
    std::size_t n) const noexcept
{
    auto it0 = end();
    --it0;
    if(n <= it0->size())
        return {
            sans_prefix(*it0, it0->size() - n),
            mutable_buffer{} };
    n -= it0->size();
    auto it1 = it0;
    --it1;
    if(n < it1->size())
        return {
            sans_prefix(*it1, it1->size() - n),
            *it0 };
    return *this;
}

void
mutable_buffer_pair::
slice_impl(
    slice_how how,
    std::size_t n) noexcept
{
    switch(how)
    {
    case slice_how::trim_front:
    {
        auto p = &b_[0];
        if(n < p->size())
        {
            trim_front(*p, n);
            return;
        }
        n -= p->size();
        *p = b_[1];
        b_[1] = {};
        trim_front(*p, n);
        return;
    }

    case slice_how::trim_back:
    {
        auto p = &b_[1];
        if(n < p->size())
        {
            trim_back(*p, n);
            return;
        }
        n -= p->size();
        *p-- = {};
        trim_back(*p, n);
        return;
    }

    case slice_how::keep_front:
    {
        auto p = &b_[0];
        if(n <= p->size())
        {
            keep_front(*p, n);
            b_[1] = {};
            return;
        }
        n -= p->size();
        ++p;
        keep_front(*p, n);
        return;
    }

    case slice_how::keep_back:
    {
        auto p = &b_[1];
        if(n <= p->size())
        {
            b_[0] = *p;
            b_[1] = {};
            keep_back(*--p, n);
            return;
        }
        n -= p->size();
        keep_back(*--p, n);
        return;
    }
    }
}

} // buffers
} // boost
