set(SC_AGENTS_COMMON_SRC "${CMAKE_CURRENT_LIST_DIR}")

file(GLOB SOURCES CONFIGURE_DEPENDS
    "*.cpp" "*.hpp"
    "*/*.cpp" "*/*.hpp"
)

find_package(nlohmann_json 3.2.0 REQUIRED)

add_library(sc-agents-common SHARED ${SOURCES})
target_link_libraries(sc-agents-common
    LINK_PRIVATE sc-memory
    LINK_PUBLIC nlohmann_json::nlohmann_json
)
target_include_directories(sc-agents-common
    PUBLIC ${SC_AGENTS_COMMON_SRC}
    PRIVATE ${SC_MEMORY_SRC}
)
sc_codegen_ex(sc-agents-common ${SC_AGENTS_COMMON_SRC} "${SC_AGENTS_COMMON_SRC}/generated")

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-agents-common)
endif()
