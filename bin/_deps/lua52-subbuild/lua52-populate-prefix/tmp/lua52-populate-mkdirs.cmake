# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/home/runner/work/ElunaTrinityWotlk/ElunaTrinityWotlk/bin/_deps/lua52-src")
  file(MAKE_DIRECTORY "/home/runner/work/ElunaTrinityWotlk/ElunaTrinityWotlk/bin/_deps/lua52-src")
endif()
file(MAKE_DIRECTORY
  "/home/runner/work/ElunaTrinityWotlk/ElunaTrinityWotlk/bin/_deps/lua52-build"
  "/home/runner/work/ElunaTrinityWotlk/ElunaTrinityWotlk/bin/_deps/lua52-subbuild/lua52-populate-prefix"
  "/home/runner/work/ElunaTrinityWotlk/ElunaTrinityWotlk/bin/_deps/lua52-subbuild/lua52-populate-prefix/tmp"
  "/home/runner/work/ElunaTrinityWotlk/ElunaTrinityWotlk/bin/_deps/lua52-subbuild/lua52-populate-prefix/src/lua52-populate-stamp"
  "/home/runner/work/ElunaTrinityWotlk/ElunaTrinityWotlk/bin/_deps/lua52-subbuild/lua52-populate-prefix/src"
  "/home/runner/work/ElunaTrinityWotlk/ElunaTrinityWotlk/bin/_deps/lua52-subbuild/lua52-populate-prefix/src/lua52-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/runner/work/ElunaTrinityWotlk/ElunaTrinityWotlk/bin/_deps/lua52-subbuild/lua52-populate-prefix/src/lua52-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/runner/work/ElunaTrinityWotlk/ElunaTrinityWotlk/bin/_deps/lua52-subbuild/lua52-populate-prefix/src/lua52-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
