
add_executable(sc-memory-test test.cpp)
target_link_libraries(sc-memory-test sc-memory)

add_executable(test_threading test_threading.cpp)
target_link_libraries(test_threading sc-memory)


# cpp tests
set(SC_MEMORY_WRAP_TEST "${SC_MACHINE_ROOT}/sc-memory/test/wrap")

set(SC_KPM_PYTHON_PATH "${SC_MACHINE_ROOT}/sc-kpm/python")

configure_file("${SC_MEMORY_WRAP_TEST}/test_defines.hpp.in" "${CMAKE_CURRENT_BINARY_DIR}/test_wrap_gen/test_defines.hpp")

set(SOURCES_WRAP_TEST
	"wrap/test_wrap.cpp"
	)

file(GLOB UnitsGlob "wrap/units/*.cpp" "wrap/units/*.hpp")

add_executable(test_wrap ${SOURCES_WRAP_TEST} ${UnitsGlob})
target_link_libraries(test_wrap sc-memory-cpp)
sc_codegen(test_wrap ${SC_MEMORY_WRAP_TEST})
