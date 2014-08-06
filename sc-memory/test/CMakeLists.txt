
add_executable(test test.cpp)
include_directories(${SC_MEMORY_SRC} ${GLIB2_INCLUDE_DIRS})
target_link_libraries(test sc-memory)


add_executable(test_threading test_threading.cpp)
include_directories(${SC_MEMORY_SRC} ${GLIB2_INCLUDE_DIRS})
target_link_libraries(test_threading sc-memory)
