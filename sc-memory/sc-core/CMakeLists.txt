file(GLOB SOURCES CONFIGURE_DEPENDS
    "src/*.h" "src/*.c"
    "src/*/*.h" "src/*/*.c"
    "src/*/*/*.h" "src/*/*/*.c"
    "include/sc-core/*.h"
)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLIB2 REQUIRED glib-2.0)
pkg_search_module(GLIB2_MODULE REQUIRED gmodule-2.0)

set(GLIB2_LIBRARIES ${GLIB_LDFLAGS} ${GLIB2_MODULE_LDFLAGS})

add_library(sc-core SHARED ${SOURCES})
target_link_libraries(sc-core LINK_PRIVATE ${GLIB2_LIBRARIES})
target_include_directories(sc-core
    PRIVATE ${GLIB2_INCLUDE_DIRS}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-core)
endif()

if(${SC_BUILD_TESTS})
    add_subdirectory(tests)
endif()
