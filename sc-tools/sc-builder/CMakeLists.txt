set(SC_BUILDER_SRC "${CMAKE_CURRENT_LIST_DIR}")

configure_file(
    "${SC_BUILDER_SRC}/src/builder_defines.hpp.in"
    "${SC_BUILDER_SRC}/src/builder_defines.hpp"
)

file(GLOB SOURCES CONFIGURE_DEPENDS "src/*.cpp" "src/*.hpp")
list(REMOVE_ITEM SOURCES "${SC_BUILDER_SRC}/src/main.cpp")

add_library(sc-builder-lib SHARED ${SOURCES})
target_link_libraries(sc-builder-lib
    LINK_PUBLIC sc-memory
    LINK_PUBLIC sc-config-utils
)
target_include_directories(sc-builder-lib
    PUBLIC ${SC_BUILDER_SRC}
)

add_executable(sc-builder "${SC_BUILDER_SRC}/src/main.cpp")
target_link_libraries(sc-builder LINK_PRIVATE sc-builder-lib)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-builder)
    target_clangformat_setup(sc-builder-lib)
endif()

if(${SC_BUILD_TESTS})
    include(tests/tests.cmake)
endif()
