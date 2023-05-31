set(SC_SERVER_ROOT ${CMAKE_CURRENT_LIST_DIR})

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h" "src/*.hpp")
list(REMOVE_ITEM SOURCES
    "${SC_SERVER_ROOT}/src/main.cpp"
    "${SC_SERVER_ROOT}/src/sc_server_factory.cpp"
    "${SC_SERVER_ROOT}/src/sc_server_factory.hpp"
)

add_library(sc-server-lib SHARED ${SOURCES})
include_directories(${SC_MEMORY_SRC} ${SC_CONFIG_UTILS_SRC} ${GLIB2_INCLUDE_DIRS})
target_link_libraries(sc-server-lib PRIVATE sc-memory nlohmann_json::nlohmann_json websocketpp::websocketpp)

add_executable(sc-server
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/sc_server_factory.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/sc_server_factory.hpp"
)
target_link_libraries(sc-server PRIVATE sc-server-lib sc-config-utils)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-server-lib)
    target_clangformat_setup(sc-server)
endif()

install_targets("/bin" sc-server sc-server-lib)

if(${SC_BUILD_TESTS})
    configure_file(
        "${SC_SERVER_ROOT}/tests/test_defines.hpp.in"
        "${SC_SERVER_ROOT}/tests/test_defines.hpp"
    )
    include(tests/api/tests.cmake)
endif()

if(${SC_BUILD_BENCH})
    add_subdirectory(tests/performance)
endif()
