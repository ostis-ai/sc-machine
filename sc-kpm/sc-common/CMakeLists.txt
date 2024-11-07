file(GLOB SOURCES CONFIGURE_DEPENDS
    "src/*.h" "src/*.c"
    "src/*/*.h" "src/*/*.c"
    "include/sc-common/*.h"
)

add_library(sc-kpm-common SHARED ${SOURCES})
target_link_libraries(sc-kpm-common LINK_PUBLIC sc-core)
set_target_properties(sc-kpm-common PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${SC_EXTENSIONS_DIRECTORY})

target_include_directories(sc-kpm-common
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PUBLIC $<INSTALL_INTERFACE:include>
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-kpm-common)
endif()
