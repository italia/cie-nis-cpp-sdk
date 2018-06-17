cmake_minimum_required(VERSION 3.5)
project(cienis
  VERSION 1.0.0
  LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FindPkgConfig)
pkg_check_modules(CRYPTOPP REQUIRED libcrypto++)
pkg_check_modules(PCSCLITE REQUIRED libpcsclite)
pkg_check_modules(SSL REQUIRED libssl)

find_package(Doxygen)
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

option(BUILD_DOCUMENTATION
  "Create and install the HTML based API documentation (requires Doxygen)"
  ${DOXYGEN_FOUND})

add_subdirectory(docs)
add_subdirectory(src)
