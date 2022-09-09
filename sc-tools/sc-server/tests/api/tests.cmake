make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/units
    NAME sc-server-tests
    DEPENDS sc-memory sc-server-lib sc-config-utils
    INCLUDES ${SC_MEMORY_SRC} ${GLIB2_LIBRARIES}
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-server-tests)
endif()
