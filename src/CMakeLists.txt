add_subdirectory(arch/posix)
add_subdirectory(asn)
add_subdirectory(test)
add_subdirectory(util)

add_library(cienis STATIC nis.cpp)
target_include_directories(cienis
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>)
target_link_libraries(cienis
  PRIVATE
    nis_manager
    reader
    reader_pcsc
    requests
    token_pcsc
  PUBLIC
    token)
install(TARGETS cienis DESTINATION lib)

add_library(digest STATIC digest.cpp)
target_include_directories(digest
  PUBLIC
    ${CRYPTOPP_INCLUDE_DIRS})
target_link_libraries(digest
  PUBLIC
    util_array
    ${CRYPTOPP_LIBRARIES})

add_library(nis_manager STATIC nis_manager.cpp)
target_link_libraries(nis_manager
  PRIVATE
    token
  PUBLIC
    reader
    reader_pcsc
    requests
    token
    token_pcsc)

add_library(reader STATIC reader.cpp)
target_link_libraries(reader
  PUBLIC
    token)

add_library(reader_pcsc STATIC reader_pcsc.cpp)
target_include_directories(reader_pcsc
  PRIVATE
    ${PCSCLITE_INCLUDE_DIRS})
target_link_libraries(reader_pcsc
  PRIVATE
    token_pcsc
  PUBLIC
    reader
    ${PCSCLITE_LIBRARIES})

add_library(requests STATIC requests.cpp)
target_include_directories(requests
  PRIVATE
    ${PCSCLITE_INCLUDE_DIRS})
target_link_libraries(requests
  PUBLIC
    token)

add_library(token STATIC token.cpp)
target_include_directories(token
  PUBLIC
    ${PCSCLITE_INCLUDE_DIRS})
target_link_libraries(token
  PUBLIC
    util_array
  PRIVATE
    arch_posix_thread
    asn_parser
    digest
    nis_manager
    util_exception
    OpenSSL::SSL)

add_library(token_pcsc STATIC token_pcsc.cpp)
target_include_directories(token_pcsc
  PUBLIC
    ${PCSCLITE_INCLUDE_DIRS})
target_link_libraries(token_pcsc
  PUBLIC
    token
    ${PCSCLITE_LIBRARIES})
