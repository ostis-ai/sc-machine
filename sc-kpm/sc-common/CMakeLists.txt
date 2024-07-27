file(GLOB SOURCES CONFIGURE_DEPENDS
    "src/*.h" "src/*.c"
    "src/*/*.h" "src/*/*.c"
    "include/sc-common/*.h"
)

add_library(sc-kpm-common SHARED ${SOURCES})
target_link_libraries(sc-kpm-common LINK_PUBLIC sc-core)
target_include_directories(sc-kpm-common
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*.h"
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-kpm-common)
endif()
