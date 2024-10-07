file(GLOB SOURCES CONFIGURE_DEPENDS "*.cpp" "*.hpp")

add_executable(sc-memory-performance-tests ${SOURCES})

target_link_libraries(sc-memory-performance-tests
    LINK_PRIVATE sc-memory
    LINK_PRIVATE benchmark::benchmark
)
