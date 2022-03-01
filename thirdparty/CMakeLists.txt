# build antlr4 runtime
add_subdirectory(antlr/runtime)

if (${SC_BUILD_TESTS})
	include(gtest.cmake)
	include(benchmark.cmake)
endif()
