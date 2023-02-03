//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/buffers
//

#ifndef BOOST_BUFFERS_DETAIL_EXCEPT_HPP
#define BOOST_BUFFERS_DETAIL_EXCEPT_HPP

//#include <boost/buffers/error.hpp>
#include <boost/assert/source_location.hpp>
#include <boost/exception/diagnostic_information.hpp>

namespace boost {
namespace buffers {
namespace detail {

BOOST_BUFFERS_DECL void BOOST_NORETURN throw_invalid_argument(
    source_location const& loc = BOOST_CURRENT_LOCATION);

BOOST_BUFFERS_DECL void BOOST_NORETURN throw_length_error(
    source_location const& loc = BOOST_CURRENT_LOCATION);

BOOST_BUFFERS_DECL void BOOST_NORETURN throw_logic_error(
    source_location const& loc = BOOST_CURRENT_LOCATION);

} // detail
} // buffers
} // boost

#endif
