file(GLOB SOURCES CONFIGURE_DEPENDS
    "src/*.h" "src/*.c"
)

add_library(sc-kpm-utils SHARED ${SOURCES})
target_link_libraries(sc-kpm-utils LINK_PUBLIC sc-kpm-common)
target_include_directories(sc-kpm-utils PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-kpm-utils)
endif()

if(${SC_BUILD_TESTS})
    set(SC_KPM_UTILS_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src")
    add_subdirectory(tests)
endif()
