set(CIE_NIS_SOURCES
  ${CIE_NIS_SOURCES}
  ${CMAKE_CURRENT_SOURCE_DIR}/array.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/array.h
  ${CMAKE_CURRENT_SOURCE_DIR}/exception.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/exception.h
  ${CMAKE_CURRENT_SOURCE_DIR}/util.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/util.h
  PARENT_SCOPE)

# Exporting these files would be made unnecessary by the removal of
# custom array.h types.
# See: https://github.com/italia/cie-nis-cpp-sdk/issues/37
install(
  FILES array.h exception.h
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/cienis/util" COMPONENT dev)
