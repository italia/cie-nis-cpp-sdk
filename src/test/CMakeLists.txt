add_executable(testnisc example-nis-c.c)
target_link_libraries(testnisc cienis)

add_executable(testniscpp example-nis-cpp.cpp)
target_link_libraries(testniscpp cienis)

add_executable(testnissod example-nis-sod.cpp)
target_link_libraries(testnissod cienis)
