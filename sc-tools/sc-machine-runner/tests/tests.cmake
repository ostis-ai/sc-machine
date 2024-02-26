make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/units
    NAME sc-machine-tests
    DEPENDS sc-machine-setup
    INCLUDES ${SC_MEMORY_SRC}
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-machine-tests)
endif()
