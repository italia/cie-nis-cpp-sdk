add_library(asn_parser STATIC ASNParser.cpp)
target_include_directories(asn_parser
  PUBLIC
    ${PCSCLITE_INCLUDE_DIRS}
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>)
target_link_libraries(asn_parser
  PRIVATE
    util_util
  PUBLIC
    util_array)
