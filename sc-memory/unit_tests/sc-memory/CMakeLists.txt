set(SC_KPM_PYTHON_PATH "${SC_MACHINE_ROOT}/sc-kpm/sc-python/services")

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/test_defines.hpp.in"
  "${CMAKE_CURRENT_BINARY_DIR}/sc-memory-tests_gen/test_defines.hpp")

file(GLOB_RECURSE SOURCES "*.cpp" "*.hpp")

add_executable(sc-memory-tests ${SOURCES})

target_include_directories(sc-memory-tests 
  PRIVATE ${GLIB2_INCLUDE_DIRS}
	PRIVATE ${SC_MEMORY_SRC}
	PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/sc-memory-tests_gen/")

target_link_libraries(sc-memory-tests 
  sc-memory
  sc-test-framework)

sc_codegen(sc-memory-tests ${CMAKE_CURRENT_LIST_DIR})
