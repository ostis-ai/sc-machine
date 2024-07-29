set(SC_MACHINE_RUNNER_SRC ${CMAKE_CURRENT_LIST_DIR})

file(GLOB SOURCES CONFIGURE_DEPENDS "src/*.cpp" "src/*.hpp")
list(REMOVE_ITEM SOURCES "${SC_MACHINE_RUNNER_SRC}/src/main.cpp")

add_library(sc-machine-runner SHARED ${SOURCES})
target_link_libraries(sc-machine-runner
    LINK_PUBLIC sc-memory
    LINK_PUBLIC sc-config-utils
)
target_compile_features(sc-machine-runner PUBLIC cxx_std_20)

add_executable(sc-machine "${SC_MACHINE_RUNNER_SRC}/src/main.cpp")
target_link_libraries(sc-machine LINK_PRIVATE sc-machine-runner)
target_compile_features(sc-machine PUBLIC cxx_std_20)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-machine-runner)
    target_clangformat_setup(sc-machine)
endif()

if(${SC_BUILD_TESTS})
    include(tests/tests.cmake)
endif()
