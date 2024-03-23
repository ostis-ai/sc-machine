set(SC_MACHINE_RUNNER_ROOT ${CMAKE_CURRENT_LIST_DIR})

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h" "src/*.hpp")
list(REMOVE_ITEM SOURCES "${SC_MACHINE_RUNNER_ROOT}/src/main.cpp")

add_library(sc-machine-runner SHARED ${SOURCES})
target_link_libraries(sc-machine-runner
    LINK_PRIVATE sc-memory
    LINK_PRIVATE sc-config-utils
)
target_include_directories(sc-machine-runner
    PUBLIC ${SC_MEMORY_SRC}
    PUBLIC ${SC_CONFIG_UTILS_SRC}
)

add_executable(sc-machine "${SC_MACHINE_RUNNER_ROOT}/src/main.cpp")
target_link_libraries(sc-machine LINK_PRIVATE sc-machine-runner)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-machine-runner)
    target_clangformat_setup(sc-machine)
endif()

configure_file(
    "${SC_MACHINE_RUNNER_ROOT}/tests/test_defines.hpp.in"
    "${SC_MACHINE_RUNNER_ROOT}/tests/test_defines.hpp"
)

if(${SC_BUILD_TESTS})
    include(tests/tests.cmake)
endif()
