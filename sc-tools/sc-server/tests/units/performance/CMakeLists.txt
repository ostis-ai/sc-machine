file(GLOB SOURCES CONFIGURE_DEPENDS
    "*.cpp" "*.hpp"
    "*/*.cpp" "*/*.hpp"
)

add_executable(sc-server-benchmarks ${SOURCES})
target_link_libraries(sc-server-benchmarks
    LINK_PRIVATE sc-server-lib
    LINK_PRIVATE benchmark::benchmark
)
target_include_directories(sc-server-benchmarks
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-server-benchmarks)
endif()
