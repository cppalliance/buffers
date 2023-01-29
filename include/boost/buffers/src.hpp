//
// Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/buffers
//

#ifndef BOOST_BUFFERS_SRC_HPP
#define BOOST_BUFFERS_SRC_HPP

/*

This file is meant to be included once,
in a translation unit of the program.

*/

#ifndef BOOST_BUFFERS_SOURCE
#define BOOST_BUFFERS_SOURCE
#endif

#ifdef BOOST_BUFFERS_URL
#include <boost/url/url.hpp>
#endif

#include <boost/buffers/detail/config.hpp>

#include <boost/buffers/detail/impl/except.ipp>

#include <boost/buffers/impl/circular_buffer.ipp>

#endif
