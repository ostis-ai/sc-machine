make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/units
    NAME scs-tests
    DEPENDS sc-memory
    INCLUDES ${SC_MEMORY_SRC} ${CMAKE_CURRENT_LIST_DIR}/units
)
