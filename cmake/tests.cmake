find_package(GTest REQUIRED)
enable_testing()

function(make_tests_from_folder folder)
  set(SINGLE_ARGS NAME)
  set(MULTI_ARGS DEPENDS INCLUDES ARGUMENTS)

  cmake_parse_arguments(TEST "" "${SINGLE_ARGS}" "${MULTI_ARGS}" ${ARGN})

  set(target "${TEST_NAME}")

  message(STATUS "Create test ${target}")

  file(GLOB_RECURSE SOURCES "${folder}/*.cpp" "${folder}/*.hpp")

  add_executable(${target} ${SOURCES})
  target_link_libraries(${target} ${GTEST_MAIN_LIBRARIES} ${TEST_DEPENDS})
  target_include_directories(${target} PRIVATE ${GTEST_INCLUDE_DIRS} ${TEST_INCLUDES})
  target_compile_features(sc-memory PRIVATE cxx_std_20)

  add_test(NAME ${target} COMMAND ${target} ${TEST_ARGUMENTS})
endfunction()
