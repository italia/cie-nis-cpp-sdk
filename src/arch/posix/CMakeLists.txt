add_library(arch_posix_thread STATIC thread.cpp)
target_link_libraries(arch_posix_thread
  PUBLIC
    Threads::Threads)
