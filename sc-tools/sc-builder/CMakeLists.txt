file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h" "src/*.hpp")

list(REMOVE_ITEM SOURCES
    "${SC_BUILDER_SRC}/src/main.cpp"
)

add_executable(sc-builder
    "${SC_BUILDER_SRC}/src/main.cpp"
)

add_library(sc-builder-lib SHARED ${SOURCES})
include_directories(${SC_MEMORY_SRC} ${SC_CONFIG_UTILS_SRC} ${GLIB2_INCLUDE_DIRS})
target_link_libraries(sc-builder-lib sc-memory)
target_link_libraries(sc-builder sc-builder-lib sc-config-utils)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-builder)
endif()

install_targets("/bin" sc-builder sc-builder-lib)
sc_codegen(sc-builder-lib ${SC_BUILDER_SRC}/src)

if(${SC_BUILD_TESTS})
    include(tests/tests.cmake)
endif()
