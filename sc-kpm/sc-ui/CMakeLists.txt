file(GLOB SOURCES CONFIGURE_DEPENDS
    "*.cpp" "*.h"
    "*/*.cpp" "*/*.h"
)

find_package(nlohmann_json 3.2.0 REQUIRED)

add_library(sc-ui SHARED ${SOURCES})
target_link_libraries(sc-ui
    LINK_PRIVATE sc-memory
    LINK_PRIVATE sc-kpm-common
    LINK_PUBLIC nlohmann_json::nlohmann_json
)
target_include_directories(sc-ui
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE ${SC_MEMORY_SRC}
    PRIVATE ${SC_COMMON_SRC}
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-ui)
endif()
