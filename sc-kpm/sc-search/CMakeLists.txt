file(GLOB SOURCES CONFIGURE_DEPENDS
    "src/*.h" "src/*.c"
    "src/*/*.h" "src/*/*.c"
)

add_library(sc-kpm-search SHARED ${SOURCES})
target_link_libraries(sc-kpm-search LINK_PUBLIC sc-kpm-common)
target_include_directories(sc-kpm-search PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-kpm-search)
endif()

if(${SC_BUILD_TESTS})
    set(SC_KPM_SEARCH_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src")
    add_subdirectory(tests)
endif()
