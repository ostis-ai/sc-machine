include_directories(${SC_MEMORY_SRC} ${GLIB2_INCLUDE_DIRS})

add_executable(test test.cpp)
target_link_libraries(test sc-memory)

add_executable(test_threading test_threading.cpp)
target_link_libraries(test_threading sc-memory)

add_executable(test_wrap test_wrap.cpp)
target_link_libraries(test_wrap sc-memory-cpp)
