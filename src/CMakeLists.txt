set(CIE_NIS_SOURCES
  nis.cpp
  nis.h
  digest.cpp
  digest.h
  executor.h
  nis_manager.cpp
  nis_manager.h
  nis_types.h
  token.cpp
  token.h
  token_pcsc.cpp
  token_pcsc.h
  reader.cpp
  reader.h
  reader_pcsc.cpp
  reader_pcsc.h
  requests.cpp
  requests.h)

add_subdirectory(arch/posix)
add_subdirectory(asn)
add_subdirectory(test)
add_subdirectory(util)

add_library(cienis
  ${CIE_NIS_SOURCES})

target_include_directories(cienis
  PUBLIC
    ${CRYPTOPP_INCLUDE_DIRS}
    ${PCSCLITE_INCLUDE_DIRS}
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>)

target_link_libraries(cienis
  PUBLIC
    OpenSSL::SSL
    Threads::Threads
    ${CRYPTOPP_LIBRARIES}
    ${PCSCLITE_LIBRARIES})
