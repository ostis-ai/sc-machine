
add_subdirectory(sc-core)
add_subdirectory(sc-memory)

#[[if (${SC_BUILD_TESTS})]]
  # add_subdirectory(sc-test-agents)
  message("=====================================================================")
  add_subdirectory(sc-test-framework)
  add_subdirectory(unit_tests)
#[[
endif ()]]
