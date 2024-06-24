set(SC_AGENTS_COMMON_SRC "${CMAKE_CURRENT_LIST_DIR}")

file(GLOB SOURCES CONFIGURE_DEPENDS
    "*.cpp" "*.hpp"
    "*/*.cpp" "*/*.hpp"
)

add_library(sc-agents-common SHARED ${SOURCES})
target_link_libraries(sc-agents-common
    LINK_PUBLIC sc-memory
    LINK_PRIVATE nlohmann_json::nlohmann_json
)
target_include_directories(sc-agents-common
    PRIVATE "${SC_AGENTS_COMMON_SRC}"
    PUBLIC "${SC_AGENTS_COMMON_SRC}/.."
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-agents-common)
endif()
