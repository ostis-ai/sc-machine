make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/units
    NAME scs-tests
    DEPENDS sc-memory
    INCLUDES ${CMAKE_CURRENT_LIST_DIR}/units
)
