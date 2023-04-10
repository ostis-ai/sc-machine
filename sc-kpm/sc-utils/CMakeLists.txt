file(GLOB_RECURSE SOURCES "*.c" "*.h")

add_library(sc-utils SHARED ${SOURCES})

target_include_directories(sc-utils
    PRIVATE ${SC_MEMORY_SRC}
    PRIVATE ${GLIB2_INCLUDE_DIRS}
)
add_dependencies(sc-utils sc-core sc-kpm-common sc-search)
target_link_libraries(sc-utils sc-kpm-common sc-search)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-utils)
endif()
