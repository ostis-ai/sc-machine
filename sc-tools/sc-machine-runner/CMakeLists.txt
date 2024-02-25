set(SC_MACHINE_RUNNER_ROOT ${CMAKE_CURRENT_LIST_DIR})

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h" "src/*.hpp")
list(REMOVE_ITEM SOURCES "${SC_MACHINE_RUNNER_ROOT}/src/main.cpp")

add_library(sc-machine-runner SHARED ${SOURCES})
include_directories(${SC_MEMORY_SRC} ${SC_CONFIG_UTILS_SRC} ${GLIB2_INCLUDE_DIRS})
target_link_libraries(sc-machine-runner sc-memory sc-config-utils)

add_executable(sc-machine "${SC_MACHINE_RUNNER_ROOT}/src/main.cpp")
target_link_libraries(sc-machine sc-machine-runner)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-machine-runner)
    target_clangformat_setup(sc-machine)
endif()
