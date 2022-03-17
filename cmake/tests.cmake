function(make_tests_from_folder folder)

  set (SINGLE_ARGS NAME)
  set (MULTI_ARGS DEPENDS INCLUDES ARGUMENTS)

  cmake_parse_arguments(TEST "" "${SINGLE_ARGS}" "${MULTI_ARGS}" ${ARGN})

  set (target "${TEST_NAME}")

  message(STATUS "Create test ${target}")

  file (GLOB_RECURSE SOURCES 
        "${folder}/*.cpp"
        "${folder}/*.hpp")

  add_executable(${target} ${SOURCES})
  target_link_libraries(${target} gtest ${TEST_DEPENDS})
  target_include_directories(${target} PRIVATE ${TEST_INCLUDES})

  add_test(NAME ${target} COMMAND ${target} ${TEST_ARGUMENTS})

endfunction()
