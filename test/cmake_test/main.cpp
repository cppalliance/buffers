//
// Copyright (c) 2023 Christian Mazakas
//
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
//

#include <boost/buffers.hpp>
#include <boost/buffers/src.hpp>

int main()
{
    boost::buffers::flat_buffer fbuf;
    if (fbuf.size() != 0) {
      throw 1234;
    }
    return 0;
}
