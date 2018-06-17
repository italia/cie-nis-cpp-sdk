add_library(util_array STATIC array.cpp)
target_include_directories(util_array
  PRIVATE
    OpenSSL::SSL
  PUBLIC
    util_exception
    util_util
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>)

add_library(util_exception STATIC exception.cpp)
target_include_directories(util_exception
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>)

add_library(util_util STATIC util.cpp)
target_include_directories(util_util
  PUBLIC
    ${PCSCLITE_INCLUDE_DIRS}
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>)
target_link_libraries(util_util
  PRIVATE
    util_exception
  PUBLIC
    util_array)
