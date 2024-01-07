//
// Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/buffers
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

#include <boost/buffers/detail/config.hpp>

#include <boost/buffers/detail/impl/except.ipp>

#include <boost/buffers/impl/circular_buffer.ipp>
#include <boost/buffers/impl/const_buffer_pair.ipp>
#include <boost/buffers/impl/const_buffer_subspan.ipp>
#include <boost/buffers/impl/mutable_buffer_pair.ipp>
#include <boost/buffers/impl/mutable_buffer_subspan.ipp>

//------------------------------------------------

// Trick boostdep into requiring URL
// since we need it for the unit tests
#ifdef BOOST_BUFFERS_BOOSTDEP
#include <boost/url/url.hpp>
#endif

#endif
