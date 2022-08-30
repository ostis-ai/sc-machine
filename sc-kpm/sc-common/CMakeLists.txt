set(SC_COMMON_SRC ${CMAKE_CURRENT_LIST_DIR})

set(SOURCES "sc_keynodes.c")

set(HEADERS "sc_keynodes.h" "sc_common_types.h")

add_library(sc-kpm-common SHARED ${SOURCES} ${HEADERS})

target_include_directories(sc-kpm-common
    PRIVATE ${SC_COMMON_SRC}
    PRIVATE ${SC_MEMORY_SRC}
    PRIVATE ${GLIB2_INCLUDE_DIRS}
)

add_dependencies(sc-kpm-common sc-core)
target_link_libraries(sc-kpm-common sc-core ${GLIB2_LIBRARIES})
target_compile_definitions(sc-kpm-common PUBLIC "-DSC_KPM_COMMON_SELF_BUILD")

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-kpm-common)
endif()
