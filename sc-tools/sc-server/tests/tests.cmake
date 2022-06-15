configure_file(
  "${CMAKE_CURRENT_LIST_DIR}/units/test_defines.hpp.in"
  "${CMAKE_CURRENT_LIST_DIR}/units/test_defines.hpp")

make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/units
  NAME sc-server-tests
  DEPENDS sc-memory sc-server-lib
  INCLUDES ${SC_MEMORY_SRC} ${GLIB2_LIBRARIES})
