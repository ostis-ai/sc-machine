make_tests_from_folder(${CMAKE_CURRENT_SOURCE_DIR}/units
    NAME sc-kpm-search-utils-tests
    DEPENDS sc-memory sc-kpm-search
    INCLUDES ${SC_KPM_SRC}/sc-search/src
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-kpm-search-utils-tests)
endif()
