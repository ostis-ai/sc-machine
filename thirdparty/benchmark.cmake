set(GOOGLETEST_PATH ${CMAKE_CURRENT_LIST_DIR}/gtest)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Disable google tests" FORCE)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "Disable google tests" FORCE)

add_subdirectory(benchmark)
