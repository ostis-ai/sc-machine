file(GLOB SOURCES CONFIGURE_DEPENDS "*.cpp" "*.hpp")

add_executable(sc-memory-benchmarks ${SOURCES})

target_link_libraries(sc-memory-benchmarks
    LINK_PRIVATE sc-memory
    LINK_PRIVATE benchmark::benchmark
)
