make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/sc-agents
    NAME sc-kpm-core-agents-tests
    DEPENDS sc-memory sc-search
    INCLUDES ${SC_MEMORY_SRC}/tests/sc-memory/_test
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-kpm-core-agents-tests)
endif()

make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/sc-common-utils
    NAME sc-kpm-agent-common-utils-tests
    DEPENDS sc-agents-common
    INCLUDES ${SC_MEMORY_SRC}/tests/sc-memory/_test
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-kpm-agent-common-utils-tests)
endif()

make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/sc-utils
    NAME sc-kpm-utils-tests
    DEPENDS sc-memory sc-utils sc-search
    INCLUDES ${SC_MEMORY_SRC}/tests/sc-memory/_test
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-kpm-utils-tests)
endif()

make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/translators
    NAME sc-kpm-translators-tests
    DEPENDS sc-ui
    INCLUDES ${SC_MEMORY_SRC}/tests/sc-memory/_test
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-kpm-translators-tests)
endif()

add_definitions(-DSC_KPM_TEST_SRC_PATH="${CMAKE_CURRENT_LIST_DIR}")
