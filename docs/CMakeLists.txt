if(BUILD_DOCUMENTATION)
  if(NOT DOXYGEN_FOUND)
      message(FATAL_ERROR "Doxygen is needed to build the documentation.")
  endif()

  set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
  set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
  configure_file(${doxyfile_in} ${doxyfile} @ONLY)

  add_custom_target(docs ALL
    COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM)

  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION share/doc)
  # Doxygen creates "Directory Reference" files as part of building the man
  # pages. Those are unwanted and are named after the local file path, so we
  # exclude them from the install step.
  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/man DESTINATION share
          PATTERN "man/man3/_*" EXCLUDE)
endif()
