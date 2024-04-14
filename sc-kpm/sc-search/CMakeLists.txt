file(GLOB SOURCES CONFIGURE_DEPENDS
    "*.c" "*.h"
    "*/*.c" "*/*.h"
)

add_library(sc-search SHARED ${SOURCES})
target_link_libraries(sc-search LINK_PUBLIC sc-kpm-common)
target_include_directories(sc-search
    PRIVATE "${CMAKE_CURRENT_LIST_DIR}"
    PUBLIC "${CMAKE_CURRENT_LIST_DIR}/.."
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-search)
endif()
