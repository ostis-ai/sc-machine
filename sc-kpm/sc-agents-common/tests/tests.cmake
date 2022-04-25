file(GLOB_RECURSE SOURCES
		"tests/keynodes/*")
add_library(test_keynodes SHARED ${SOURCES})
sc_codegen_ex(test_keynodes ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/generated)

make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/units
		NAME sc-agents-common-tests
		DEPENDS sc-agents-common test_keynodes
		INCLUDES ${SC_MEMORY_SRC} ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/.. )

add_definitions(-DSC_AGENTS_COMMON_TEST_SRC_PATH="${CMAKE_CURRENT_LIST_DIR}/TestStructures")

add_definitions (-DSC_AGENTS_COMMON_REPO_PATH="${SC_BIN_PATH}/sc-agents-common-test-repo")
