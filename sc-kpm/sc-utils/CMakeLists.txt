file(GLOB SOURCES CONFIGURE_DEPENDS "*.c" "*.h")

add_library(sc-utils SHARED ${SOURCES})
target_link_libraries(sc-utils
    LINK_PRIVATE sc-memory
    LINK_PRIVATE sc-kpm-common
    LINK_PRIVATE sc-search
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-utils)
endif()
