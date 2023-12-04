#
# Copyright (c) 2023 Christian Mazakas
# Copyright (c) 2022 alandefreitas (alandefreitas@gmail.com)
#
# Distributed under the Boost Software License, Version 1.0.
# https://www.boost.org/LICENSE_1_0.txt
#

cmake_minimum_required(VERSION 3.5...3.16)

project(cmake_subdir_test LANGUAGES CXX)
set(__ignore__ ${CMAKE_C_COMPILER})
set(__ignore__ ${CMAKE_C_FLAGS})

if(BOOST_CI_INSTALL_TEST)
  find_package(Boost CONFIG REQUIRED COMPONENTS url buffers)
else()

  set(BOOST_BUFFERS_BUILD_TESTS OFF CACHE BOOL "Build the tests." FORCE)
  add_subdirectory(../.. boostorg/buffers)

  # set the project version here to suppress URL's detection that causes it to
  # re-add all of Boost which errors out with duplicated targets
  #
  set(BOOST_SUPERPROJECT_VERSION "1.84.0")

  set(deps
    # Primary dependencies

    assert
    config
    static_assert
    throw_exception
    url

    # Secondary dependencies

    align
    core
    mp11
    optional
    system
    type_traits
    variant2
    detail
    move
    predef
    utility
    winapi
    preprocessor
    io
  )
  set(BUILD_TESTING OFF CACHE BOOL "Build the tests." FORCE)
  foreach(dep IN LISTS deps)
    add_subdirectory(../../../${dep} boostorg/${dep} EXCLUDE_FROM_ALL)
  endforeach()
  set(BUILD_TESTING ON CACHE BOOL "Build the tests." FORCE)
endif()

add_executable(main main.cpp)
target_link_libraries(main Boost::buffers)

enable_testing()
add_test(NAME main COMMAND main)
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -C $<CONFIG>)
