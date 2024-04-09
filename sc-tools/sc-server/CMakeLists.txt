set(SC_SERVER_ROOT ${CMAKE_CURRENT_LIST_DIR})

file(GLOB SOURCES CONFIGURE_DEPENDS
    "src/*.cpp" "src/*.hpp"
    "src/*/*.cpp" "src/*/*.hpp"
    "src/*/*/*.cpp" "src/*/*/*.hpp"
    "src/*/*/*/*.cpp" "src/*/*/*/*.hpp"
)
list(REMOVE_ITEM SOURCES "${SC_SERVER_ROOT}/src/main.cpp")

find_package(nlohmann_json 3.2.0 REQUIRED)

add_library(sc-server-lib SHARED ${SOURCES})
target_link_libraries(sc-server-lib
    LINK_PRIVATE sc-memory
    LINK_PUBLIC sc-config-utils
    LINK_PUBLIC websocketpp::websocketpp
    LINK_PUBLIC nlohmann_json::nlohmann_json
)
target_include_directories(sc-server-lib
    PUBLIC ${SC_MEMORY_SRC}
    PUBLIC ${SC_CONFIG_UTILS_SRC}
)
set(SC_SERVER_MODULE_ROOT "${CMAKE_CURRENT_LIST_DIR}/src/sc-server-module")
sc_codegen_ex(sc-server-lib "${SC_SERVER_MODULE_ROOT}" "${SC_SERVER_MODULE_ROOT}/generated")

add_executable(sc-server "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
target_link_libraries(sc-server LINK_PRIVATE sc-server-lib)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-server-lib)
    target_clangformat_setup(sc-server)
endif()

configure_file(
    "${SC_SERVER_ROOT}/tests/test_defines.hpp.in"
    "${SC_SERVER_ROOT}/tests/test_defines.hpp"
)

if(${SC_BUILD_TESTS})
    include(tests/api/tests.cmake)
endif()

if(${SC_BUILD_BENCH})
    add_subdirectory(tests/performance)
endif()
