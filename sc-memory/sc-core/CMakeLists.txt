file(GLOB SOURCES CONFIGURE_DEPENDS
    "src/*.h" "src/*.c"
    "src/*/*.h" "src/*/*.c"
    "src/*/*/*.h" "src/*/*/*.c"
    "include/sc-core/*.h"
)

add_library(sc-core SHARED ${SOURCES})
target_link_libraries(sc-core LINK_PUBLIC ${GLIB2_LIBRARIES})
target_include_directories(sc-core
    PUBLIC ${GLIB2_INCLUDE_DIRS}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-core)
endif()

if(${SC_BUILD_TESTS})
    add_subdirectory(tests)
endif()
