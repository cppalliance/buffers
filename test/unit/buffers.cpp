//
// Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/buffers
//

// Test that header file is self-contained.
#include <boost/buffers.hpp>

/*

BufferSequence
    Type Traits
        is_const_buffer_sequence
            (mutable sequences are also const sequences?)
        is_mutable_buffer_sequence
    Algorithms
        begin
        empty
        end
        front
        size
        prefix
        sans_prefix
        sans_suffix
        suffix

*/
