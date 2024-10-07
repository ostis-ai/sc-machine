make_tests_from_folder(${CMAKE_CURRENT_SOURCE_DIR}/units
    NAME sc-kpm-ui-translators-tests
    DEPENDS sc-memory sc-kpm-ui
    INCLUDES ${SC_KPM_SRC}/sc-ui/src
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-kpm-ui-translators-tests)
endif()

add_definitions(-DSC_UI_TEST_SRC_PATH="${CMAKE_CURRENT_SOURCE_DIR}")
