set(CIE_NIS_PUBLIC_HEADERS
  nis.h
  nis_types.h
  token.h)

set(CIE_NIS_SOURCES
  nis.cpp
  digest.cpp
  digest.h
  executor.h
  nis_manager.cpp
  nis_manager.h
  token.cpp
  token_pcsc.cpp
  token_pcsc.h
  reader.cpp
  reader.h
  reader_pcsc.cpp
  reader_pcsc.h
  requests.cpp
  requests.h
  ${CIE_NIS_PUBLIC_HEADERS})

add_subdirectory(arch/posix)
add_subdirectory(asn)
add_subdirectory(test)
add_subdirectory(util)

add_library(cienis
  ${CIE_NIS_SOURCES})

set_target_properties(cienis
  PROPERTIES
    VERSION "${PROJECT_VERSION}"
    PUBLIC_HEADER "${CIE_NIS_PUBLIC_HEADERS}")

target_include_directories(cienis
  PUBLIC
    ${CRYPTOPP_INCLUDE_DIRS}
    ${PCSCLITE_INCLUDE_DIRS}
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_link_libraries(cienis
  PUBLIC
    OpenSSL::SSL
    Threads::Threads
    ${CRYPTOPP_LIBRARIES}
    ${PCSCLITE_LIBRARIES})

install(
  TARGETS cienis
  EXPORT  ${PROJECT_NAME}
  LIBRARY       DESTINATION "${CMAKE_INSTALL_LIBDIR}"            COMPONENT shlib
  ARCHIVE       DESTINATION "${CMAKE_INSTALL_LIBDIR}"            COMPONENT lib
  RUNTIME       DESTINATION "${CMAKE_INSTALL_BINDIR}"            COMPONENT bin
  PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/cienis" COMPONENT dev)
