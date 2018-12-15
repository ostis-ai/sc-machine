
add_executable(sc-memory-test test.cpp)
target_link_libraries(sc-memory-test sc-memory)

add_executable(test_threading test_threading.cpp)
target_link_libraries(test_threading sc-memory)

add_subdirectory(wrap)
add_subdirectory(scs)