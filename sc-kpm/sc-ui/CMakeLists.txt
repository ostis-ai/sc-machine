file(GLOB SOURCES CONFIGURE_DEPENDS
    "*.cpp" "*.h"
    "*/*.cpp" "*/*.h"
)

add_library(sc-ui SHARED ${SOURCES})
target_link_libraries(sc-ui
    LINK_PUBLIC sc-memory
    LINK_PUBLIC sc-kpm-common
    LINK_PUBLIC nlohmann_json::nlohmann_json
)
target_include_directories(sc-ui
    PRIVATE "${CMAKE_CURRENT_LIST_DIR}"
    PUBLIC "${CMAKE_CURRENT_LIST_DIR}/.."
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-ui)
endif()
