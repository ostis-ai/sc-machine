file(GLOB_RECURSE SOURCES "*.cpp" "*.hpp")

add_executable(sc-memory-performance-tests ${SOURCES})

target_include_directories(sc-memory-performance-tests
    PRIVATE ${GLIB2_INCLUDE_DIRS}
    PRIVATE ${SC_MEMORY_SRC}
    PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/sc-memory-tests_gen/"
)

target_link_libraries(sc-memory-performance-tests
    sc-memory
    benchmark
)

sc_codegen(sc-memory-performance-tests ${CMAKE_CURRENT_LIST_DIR})
