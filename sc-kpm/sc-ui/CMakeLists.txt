file(GLOB_RECURSE SOURCES "*.cpp" "*.h")

add_library(sc-ui SHARED ${SOURCES})

target_include_directories(sc-ui 
    PRIVATE ${SC_MEMORY_SRC}
    PRIVATE ${GLIB2_INCLUDE_DIRS}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)
add_dependencies(sc-ui sc-core sc-kpm-common)
target_link_libraries(sc-ui sc-kpm-common)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-ui)
endif()
