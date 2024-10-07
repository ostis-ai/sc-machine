make_tests_from_folder(${CMAKE_CURRENT_SOURCE_DIR}/units
    NAME sc-kpm-agent-common-utils-tests
    DEPENDS sc-agents-common
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-kpm-agent-common-utils-tests)
endif()
