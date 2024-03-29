#
# Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/CPPAlliance/buffers
#

set(EXTRAFILES
    ../../url/extra/test_main.cpp
    ../../url/extra/test_suite.hpp
    )

set(PFILES
    CMakeLists.txt
    Jamfile
    test_helpers.hpp
    algorithm.cpp
    any_dynamic_buffer.cpp
    buffer_copy.cpp
    buffer_size.cpp
    buffers.cpp
    circular_buffer.cpp
    const_buffer.cpp
    const_buffer_pair.cpp
    const_buffer_span.cpp
    const_buffer_subspan.cpp
    flat_buffer.cpp
    make_buffer.cpp
    mutable_buffer.cpp
    mutable_buffer_pair.cpp
    mutable_buffer_span.cpp
    mutable_buffer_subspan.cpp
    range.cpp
    string_buffer.cpp
    tag_invoke.cpp
    type_traits.cpp
    )

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "" FILES ${PFILES})
source_group("_extra" FILES ${EXTRAFILES})
add_executable(boost_buffers_tests ${PFILES} ${EXTRAFILES})
target_include_directories(boost_buffers_tests PRIVATE . ../../url/extra)
target_link_libraries(
    boost_buffers_tests PRIVATE
    boost_buffers
    )
add_test(NAME boost_buffers_tests COMMAND boost_buffers_tests)
