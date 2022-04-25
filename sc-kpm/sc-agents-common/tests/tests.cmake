#file(GLOB_RECURSE SOURCES
#		"keynodes/*")

make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/units
		NAME sc-agents-common-tests
		DEPENDS sc-agents-common
		INCLUDES ${SC_MEMORY_SRC} ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/.. )

add_definitions(-DSC_AGENTS_COMMON_TEST_SRC_PATH="${CMAKE_CURRENT_LIST_DIR}/TestStructures")
#sc_codegen_ex(sc-agents-common-tests ${CMAKE_CURRENT_LIST_DIR} "${CMAKE_CURRENT_LIST_DIR}/generated")

add_definitions (-DSC_AGENTS_COMMON_REPO_PATH="${SC_BIN_PATH}/sc-agents-common-test-repo")
