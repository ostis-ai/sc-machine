file(GLOB_RECURSE SOURCES "*.cpp" "*.hpp")

add_executable(sc-server-performance-tests ${SOURCES})

target_include_directories(sc-server-performance-tests
    PRIVATE ${GLIB2_INCLUDE_DIRS}
    PRIVATE ${SC_MEMORY_SRC}
    PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/sc-server-tests_gen/"
)

target_link_libraries(sc-server-performance-tests
    sc-server-lib
    sc-memory
    benchmark
)

sc_codegen(sc-server-performance-tests ${CMAKE_CURRENT_LIST_DIR})

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-server-performance-tests)
endif()
