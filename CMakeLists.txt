#
# Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/CPPAlliance/buffers
#

cmake_minimum_required(VERSION 3.8...3.16)

set(BOOST_BUFFERS_VERSION 1)
if(BOOST_SUPERPROJECT_VERSION)
    set(BOOST_BUFFERS_VERSION ${BOOST_SUPERPROJECT_VERSION})
endif()

project(boost_buffers VERSION "${BOOST_BUFFERS_VERSION}" LANGUAGES CXX)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    include(CTest)
    option(BOOST_BUFFERS_INSTALL "Install boost::buffers files" ON)
    option(BOOST_BUFFERS_BUILD_TESTS "Build boost::buffers tests" ${BUILD_TESTING})
    set(BOOST_BUFFERS_IS_ROOT ON)
else()
    set(BOOST_BUFFERS_BUILD_TESTS ${BUILD_TESTING})
    set(BOOST_BUFFERS_IS_ROOT OFF)
endif()

include(GNUInstallDirs)
if(BOOST_BUFFERS_IS_ROOT)
    set(BOOST_INCLUDE_LIBRARIES buffers)
    set(BOOST_EXCLUDE_LIBRARIES buffers)
    set(CMAKE_FOLDER Dependencies)
    add_subdirectory(../.. Dependencies/boost EXCLUDE_FROM_ALL)
    unset(CMAKE_FOLDER)
endif()

function(boost_buffers_setup_properties target)
    target_compile_features(${target} PUBLIC cxx_constexpr)
    target_compile_definitions(${target} PUBLIC BOOST_BUFFERS_NO_LIB=1)

    if(BOOST_SUPERPROJECT_VERSION)
        target_include_directories(${target} PUBLIC "${PROJECT_SOURCE_DIR}/include")
    else()
        target_include_directories(${target}
            PUBLIC
                "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
                "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
        )
    endif()

    target_link_libraries(${target}
        PUBLIC
            Boost::assert
            Boost::config
            Boost::exception
            Boost::static_assert
            Boost::system
            Boost::throw_exception
            Boost::type_traits
    )
endfunction()

file(GLOB_RECURSE BOOST_BUFFERS_HEADERS CONFIGURE_DEPENDS
    include/boost/*.hpp
    include/boost/*.ipp
    include/boost/*.natvis
)

set(BOOST_BUFFERS_SOURCES src/src.cpp)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include/boost PREFIX "" FILES ${BOOST_BUFFERS_HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "" FILES ${BOOST_BUFFERS_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "" FILES Jamfile)

add_library(boost_buffers ${BOOST_BUFFERS_HEADERS} ${BOOST_BUFFERS_SOURCES})
add_library(Boost::buffers ALIAS boost_buffers)
boost_buffers_setup_properties(boost_buffers)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(boost_buffers PUBLIC BOOST_BUFFERS_DYN_LINK=1)
else()
    target_compile_definitions(boost_buffers PUBLIC BOOST_BUFFERS_STATIC_LINK=1)
endif()

if(BOOST_BUFFERS_INSTALL AND NOT BOOST_SUPERPROJECT_VERSION)
    install(TARGETS boost_buffers
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    )
endif()

if(BOOST_BUFFERS_BUILD_TESTS)
    add_subdirectory(test)
endif()
