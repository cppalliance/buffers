//
// Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/buffers
//

#ifndef BOOST_BUFFERS_DETAIL_CONFIG_HPP
#define BOOST_BUFFERS_DETAIL_CONFIG_HPP

#include <boost/config.hpp>

namespace boost {
namespace buffers {

//------------------------------------------------

#if defined(BOOST_BUFFERS_DOCS)
# define BOOST_BUFFERS_DECL
#else
# if (defined(BOOST_BUFFERS_DYN_LINK) || defined(BOOST_ALL_DYN_LINK)) && !defined(BOOST_BUFFERS_STATIC_LINK)
#  if defined(BOOST_BUFFERS_SOURCE)
#   define BOOST_BUFFERS_DECL        BOOST_SYMBOL_EXPORT
#   define BOOST_BUFFERS_BUILD_DLL
#  else
#   define BOOST_BUFFERS_DECL        BOOST_SYMBOL_IMPORT
#  endif
# endif // shared lib

# ifndef  BOOST_BUFFERS_DECL
#  define BOOST_BUFFERS_DECL
# endif

# if !defined(BOOST_BUFFERS_SOURCE) && !defined(BOOST_ALL_NO_LIB) && !defined(BOOST_BUFFERS_NO_LIB)
#  define BOOST_LIB_NAME boost_buffers
#  if defined(BOOST_ALL_DYN_LINK) || defined(BOOST_BUFFERS_DYN_LINK)
#   define BOOST_DYN_LINK
#  endif
#  include <boost/config/auto_link.hpp>
# endif
#endif

//------------------------------------------------

#if ! defined(BOOST_BUFFERS_DOCS) && ( \
    defined(BOOST_NO_CXX14_AGGREGATE_NSDMI) || \
    defined(BOOST_MSVC) )
# define BOOST_BUFFERS_AGGREGATE_WORKAROUND
#endif

// Add source location to error codes
#ifdef BOOST_BUFFERS_NO_SOURCE_LOCATION
# define BOOST_BUFFERS_ERR(ev) (::boost::system::error_code(ev))
# define BOOST_BUFFERS_RETURN_EC(ev) return (ev)
#else
# define BOOST_BUFFERS_ERR(ev) ( \
    ::boost::system::error_code( (ev), [] { \
    static constexpr auto loc((BOOST_CURRENT_LOCATION)); \
    return &loc; }()))
# define BOOST_BUFFERS_RETURN_EC(ev) \
    static constexpr auto loc ## __LINE__((BOOST_CURRENT_LOCATION)); \
    return ::boost::system::error_code((ev), &loc ## __LINE__)
#endif

//------------------------------------------------

// avoid all of Boost.TypeTraits for just this
template<class...> struct make_void { typedef void type; };
template<class... Ts> using void_t = typename make_void<Ts...>::type;

} // buffers
} // boost

// Trick boostdep into requiring URL
// since we need it for the unit tests
#ifdef BOOST_BUFFERS_BOOSTDEP
#include <boost/url/url.hpp>
#endif

#endif
