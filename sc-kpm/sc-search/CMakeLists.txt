file(GLOB_RECURSE SOURCES "*.c" "*.cpp" "*.h" "*.hpp")

add_library(sc-search SHARED ${SOURCES})
target_link_libraries(sc-search LINK_PRIVATE sc-kpm-common)
target_include_directories(sc-search
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE ${SC_COMMON_SRC}
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-search)
endif()
