//
// Copyright (c) 2023 Christian Mazakas
//
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
//

#include <boost/buffers.hpp>
#include <boost/buffers/circular_buffer.hpp>

int main() {
  boost::buffers::flat_buffer fbuf;
  if (fbuf.size() != 0) {
    throw 1234;
  }

  boost::buffers::circular_buffer cbuf;
  auto buf_pair = cbuf.data();
  if (buf_pair[0].data() != nullptr) {
    throw 4321;
  }

  return 0;
}
