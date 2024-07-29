file(GLOB SOURCES CONFIGURE_DEPENDS
    "*.cpp" "*.hpp"
    "*/*.cpp" "*/*.hpp"
)

add_executable(sc-server-performance-tests ${SOURCES})

target_link_libraries(sc-server-performance-tests
    LINK_PRIVATE sc-server-lib
    LINK_PRIVATE benchmark::benchmark
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-server-performance-tests)
endif()
