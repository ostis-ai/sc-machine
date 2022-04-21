
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
target_link_libraries(sc-core-threading-test sc-core)

if(${SC_BUILD_BENCH})
  add_subdirectory(performance)
endif()

include (${CMAKE_CURRENT_LIST_DIR}/sc-memory/tests.cmake)
include (${CMAKE_CURRENT_LIST_DIR}/scs/tests.cmake)
