file(GLOB SOURCES CONFIGURE_DEPENDS "*.cpp" "*.hpp")

add_executable(sc-memory-performance-tests ${SOURCES})

target_link_libraries(sc-memory-performance-tests
    LINK_PRIVATE sc-memory
    LINK_PRIVATE benchmark::benchmark
)
target_include_directories(sc-memory-performance-tests
    PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/sc-memory-tests_gen/"
)

sc_codegen(sc-memory-performance-tests ${CMAKE_CURRENT_LIST_DIR})
