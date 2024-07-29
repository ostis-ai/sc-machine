set(SC_SERVER_ROOT "${CMAKE_CURRENT_LIST_DIR}")

file(GLOB SOURCES CONFIGURE_DEPENDS
    "src/*.cpp" "src/*.hpp"
    "src/*/*.cpp" "src/*/*.hpp"
    "src/*/*/*.cpp" "src/*/*/*.hpp"
    "src/*/*/*/*.cpp" "src/*/*/*/*.hpp"
)
list(REMOVE_ITEM SOURCES "${SC_SERVER_ROOT}/src/main.cpp")

add_library(sc-server-lib SHARED ${SOURCES})
target_link_libraries(sc-server-lib
    LINK_PUBLIC sc-config-utils
    LINK_PUBLIC websocketpp::websocketpp
    LINK_PUBLIC nlohmann_json::nlohmann_json
)
target_include_directories(sc-server-lib
    PUBLIC ${SC_SERVER_ROOT}
)
set_target_properties(sc-server-lib PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

set(SC_SERVER_MODULE_ROOT "${SC_SERVER_ROOT}/src/sc-server-module")
sc_codegen_ex(sc-server-lib "${SC_SERVER_MODULE_ROOT}" "${SC_SERVER_MODULE_ROOT}/generated")

add_executable(sc-server "${SC_SERVER_ROOT}/src/main.cpp")
target_link_libraries(sc-server LINK_PRIVATE sc-server-lib)
target_compile_features(sc-server PUBLIC cxx_std_17)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-server-lib)
    target_clangformat_setup(sc-server)
endif()

if(${SC_BUILD_TESTS})
    include(tests/api/tests.cmake)
endif()

if(${SC_BUILD_BENCH})
    add_subdirectory(tests/performance)
endif()
