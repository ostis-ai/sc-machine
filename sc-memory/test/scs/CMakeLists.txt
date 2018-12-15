# scs tests
set(SC_MEMORY_WRAP_TEST "${SC_MACHINE_ROOT}/sc-memory/test/scs")

set(SOURCES_SCS_TEST
	"test_scs.cpp"
	)

file(GLOB UnitsGlob "units/*.cpp" "units/*.hpp")

add_executable(test_scs ${SOURCES_SCS_TEST} ${UnitsGlob})
target_link_libraries(test_scs sc-memory-cpp)
sc_codegen(test_scs ${CMAKE_CURRENT_LIST_DIR})
