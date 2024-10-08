file(GLOB SOURCES CONFIGURE_DEPENDS
    "*.cpp" "*.hpp"
    "*/*.cpp" "*/*.hpp"
)

add_executable(sc-server-performance-tests ${SOURCES})
target_link_libraries(sc-server-performance-tests
    LINK_PRIVATE sc-server-lib
    LINK_PRIVATE benchmark::benchmark
)
target_include_directories(sc-server-performance-tests
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)
add_dependencies(sc-server-performance-tests sc-server-tests-kb-generator)
target_compile_definitions(sc-server-performance-tests PRIVATE
    SC_SERVER_REPO_PATH="${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/sc-server-test-repo"
    SC_SERVER_EXTENSIONS="${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/kb"
    SC_SERVER_INI="${CMAKE_CURRENT_SOURCE_DIR}/../../sc-server-test.ini"
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-server-performance-tests)
endif()
