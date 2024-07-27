file(GLOB SOURCES CONFIGURE_DEPENDS
    "src/*.cpp" "src/*.c"
    "include/sc-config/*.h" "include/sc-config/*.hpp" "include/sc-config/*/*.tpp"
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
    PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    PUBLIC $<INSTALL_INTERFACE:include>
)

# TODO: CMake 3.23+ supports FILE_SET (https://discourse.cmake.org/t/installing-headers-the-modern-way-regurgitated-and-revisited/3238/3) which is a more stable and portable solution. 
# For compatibility reasons we're exporting the whole folder, should be replaced in the future (Ubuntu LTS should have cmake 3.23 oob by that time)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-config)
endif()

if(${SC_BUILD_TESTS})
    add_subdirectory(tests)
endif()
