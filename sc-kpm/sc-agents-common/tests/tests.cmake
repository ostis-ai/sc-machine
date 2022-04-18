make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/units
		NAME sc-agents-common-tests
		DEPENDS sc-agents-common
		INCLUDES ${SC_MEMORY_SRC} ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/.. )

add_definitions(-DSC_AGENTS_COMMON_TEST_SRC_PATH="${CMAKE_CURRENT_LIST_DIR}/TestStructures")

add_definitions (-DSC_AGENTS_COMMON_REPO_PATH="${SC_BIN_PATH}/sc-agents-common-test-repo")
