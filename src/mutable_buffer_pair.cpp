//
// Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/buffers
//

#include <boost/buffers/mutable_buffer_pair.hpp>
#include <boost/buffers/slice.hpp>

namespace boost {
namespace buffers {

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
