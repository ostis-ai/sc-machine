
add_executable(sc-memory-test test.cpp)
target_link_libraries(sc-memory-test sc-memory)

add_executable(test_threading test_threading.cpp)
target_link_libraries(test_threading sc-memory)


set(SC_MEMORY_WRAP_TEST "${SC_MACHINE_ROOT}/sc-memory/test/wrap")
set(SOURCES_WRAP_TEST
	"wrap/test.cpp"
	"wrap/test.hpp"
	"wrap/test_wrap.cpp"
	)

file(GLOB UnitsGlob "wrap/units/*.cpp")

add_executable(test_wrap ${SOURCES_WRAP_TEST} ${UnitsGlob})
target_link_libraries(test_wrap sc-memory-cpp)
sc_codegen(test_wrap ${SC_MEMORY_WRAP_TEST})
