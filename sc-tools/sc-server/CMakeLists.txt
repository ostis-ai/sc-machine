file(GLOB SOURCES CONFIGURE_DEPENDS
    "src/*.cpp" "src/*.hpp"
    "src/*/*.cpp" "src/*/*.hpp"
    "src/*/*/*.cpp" "src/*/*/*.hpp"
    "src/*/*/*/*.cpp" "src/*/*/*/*.hpp"
)

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
    PRIVATE ${SC_CORE_SRC}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PUBLIC $<INSTALL_INTERFACE:include>
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-server-lib)
endif()

if(${SC_BUILD_TESTS})
    add_subdirectory(tests)
endif()
