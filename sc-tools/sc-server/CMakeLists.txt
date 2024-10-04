file(GLOB SOURCES CONFIGURE_DEPENDS
    "src/*.cpp" "src/*.hpp"
    "src/*/*.cpp" "src/*/*.hpp"
    "src/*/*/*.cpp" "src/*/*/*.hpp"
    "src/*/*/*/*.cpp" "src/*/*/*/*.hpp"
)
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

find_package(websocketpp REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)

add_library(sc-server-lib SHARED ${SOURCES})
target_link_libraries(sc-server-lib
    LINK_PUBLIC sc-config
    LINK_PUBLIC sc-memory
    LINK_PUBLIC websocketpp::websocketpp
    LINK_PUBLIC nlohmann_json::nlohmann_json
)
target_compile_definitions(sc-server-lib
    PUBLIC ASIO_STANDALONE
)
target_include_directories(sc-server-lib
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_executable(sc-server "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
target_link_libraries(sc-server LINK_PRIVATE sc-server-lib)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-server-lib)
    target_clangformat_setup(sc-server)
endif()

if(${SC_BUILD_TESTS})
    add_subdirectory(tests)
endif()
