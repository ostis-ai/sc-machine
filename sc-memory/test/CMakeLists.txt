
add_executable(test test.cpp)
include_directories(${SC_MEMORY_SRC} ${GLIB2_INCLUDE_DIRS})
target_link_libraries(test sc-memory)
