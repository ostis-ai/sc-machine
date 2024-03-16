configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/src/builder_defines.hpp.in"
    "${CMAKE_CURRENT_LIST_DIR}/src/builder_defines.hpp"
)

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h" "src/*.hpp")
list(REMOVE_ITEM SOURCES "${SC_BUILDER_SRC}/src/main.cpp")

add_library(sc-builder-lib SHARED ${SOURCES})
include_directories(${SC_MEMORY_SRC} ${SC_CONFIG_UTILS_SRC} ${GLIB2_INCLUDE_DIRS})
target_link_libraries(sc-builder-lib sc-memory sc-config-utils)

add_executable(sc-builder "${SC_BUILDER_SRC}/src/main.cpp")
target_link_libraries(sc-builder sc-builder-lib)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-builder)
    target_clangformat_setup(sc-builder-lib)
endif()

if(${SC_BUILD_TESTS})
    include(tests/tests.cmake)
endif()
