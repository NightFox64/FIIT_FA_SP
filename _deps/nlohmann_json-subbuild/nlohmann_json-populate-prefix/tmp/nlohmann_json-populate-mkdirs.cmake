# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/root/FIIT_FA_SP/_deps/nlohmann_json-src")
  file(MAKE_DIRECTORY "/root/FIIT_FA_SP/_deps/nlohmann_json-src")
endif()
file(MAKE_DIRECTORY
  "/root/FIIT_FA_SP/_deps/nlohmann_json-build"
  "/root/FIIT_FA_SP/_deps/nlohmann_json-subbuild/nlohmann_json-populate-prefix"
  "/root/FIIT_FA_SP/_deps/nlohmann_json-subbuild/nlohmann_json-populate-prefix/tmp"
  "/root/FIIT_FA_SP/_deps/nlohmann_json-subbuild/nlohmann_json-populate-prefix/src/nlohmann_json-populate-stamp"
  "/root/FIIT_FA_SP/_deps/nlohmann_json-subbuild/nlohmann_json-populate-prefix/src"
  "/root/FIIT_FA_SP/_deps/nlohmann_json-subbuild/nlohmann_json-populate-prefix/src/nlohmann_json-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/root/FIIT_FA_SP/_deps/nlohmann_json-subbuild/nlohmann_json-populate-prefix/src/nlohmann_json-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/root/FIIT_FA_SP/_deps/nlohmann_json-subbuild/nlohmann_json-populate-prefix/src/nlohmann_json-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
