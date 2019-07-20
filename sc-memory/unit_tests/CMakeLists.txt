
add_executable(sc-core-test test.cpp)
target_include_directories(sc-core-test 
  PRIVATE ${SC_MEMORY_SRC}
  PRIVATE ${GLIB2_INCLUDE_DIRS}
  )
target_link_libraries(sc-core-test sc-core)

add_executable(sc-core-threading-test test_threading.cpp)
target_include_directories(sc-core-threading-test 
  PRIVATE ${SC_MEMORY_SRC}
  PRIVATE ${GLIB2_INCLUDE_DIRS}
  )
target_link_libraries(sc-core-threading-test  sc-core)

add_subdirectory(scs)
add_subdirectory(sc-memory)
