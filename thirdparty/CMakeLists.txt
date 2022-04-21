# build antlr4 runtime
add_subdirectory(antlr/runtime)

if (${SC_BUILD_TESTS})
	include(gtest.cmake)
endif()

if (${SC_BUILD_BENCH})
	include(benchmark.cmake)
endif()
