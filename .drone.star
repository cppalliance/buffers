#
#  Copyright (c) 2023 Vinnie Falco (vinnie.falco@gmail.com)
#
#  Distributed under the Boost Software License, Version 1.0. (See accompanying
#  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
#  Official repository: https://github.com/CPPAlliance/buffers
#

def main(ctx):
    return generate(
        # Compilers
        ['gcc >=5.0',
         'clang >=3.8',
         'msvc >=14.1',
         'arm64-gcc latest',
         's390x-gcc latest',
         'apple-clang *',
         'arm64-clang latest',
         's390x-clang latest',
         # 'x86-msvc latest'
         ],
        # Standards
        '>=11',
        packages=[])


# from https://github.com/boostorg/boost-ci
load("@boost_ci//ci/drone/:functions.star", "linux_cxx", "windows_cxx", "osx_cxx", "freebsd_cxx")
load("@url//:.drone.star", "generate")
