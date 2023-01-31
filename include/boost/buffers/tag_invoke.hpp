//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/http_proto
//

#ifndef BOOST_BUFFERS_TAG_INVOKE_HPP
#define BOOST_BUFFERS_TAG_INVOKE_HPP

#include <boost/buffers/detail/config.hpp>

namespace boost {
namespace buffers {

/** size tag for tag_invoke.
*/
struct size_tag {};

/** prefix tag for tag_invoke.
*/
struct prefix_tag {};

/** suffix tag for tag-invoke.
*/
struct suffix_tag {};

} // buffers
} // boost

#endif
