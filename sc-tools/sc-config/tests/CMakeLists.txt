make_tests_from_folder(${CMAKE_CURRENT_SOURCE_DIR}/units
    NAME sc-config-tests
    DEPENDS sc-config
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-config-tests)
endif()
