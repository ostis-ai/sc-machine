file(GLOB SOURCES CONFIGURE_DEPENDS
    "src/*.cpp" "src/*.c"
    "src/*/*.cpp" "src/*/*.c"
    "include/sc-config/*.h" "include/sc-config/*.hpp"
    "include/sc-config/*/*.h" "include/sc-config/*/*.hpp"
)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLIB2 REQUIRED glib-2.0)
pkg_search_module(GLIB2_MODULE REQUIRED gmodule-2.0)

set(GLIB2_LIBRARIES ${GLIB_LDFLAGS} ${GLIB2_MODULE_LDFLAGS})

add_library(sc-config SHARED ${SOURCES})
target_link_libraries(sc-config
    LINK_PRIVATE ${GLIB2_LIBRARIES}
    LINK_PUBLIC sc-memory
)
target_include_directories(sc-config
    PRIVATE ${GLIB2_INCLUDE_DIRS}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-config)
endif()
