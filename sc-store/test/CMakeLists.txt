add_executable(test test.c)


include(FindPkgConfig)

pkg_check_modules (GLIB2 REQUIRED glib-2.0>=2.26)
include_directories(${SC_STORE_ROOT}/src ${GLIB2_INCLUDE_DIRS})
target_link_libraries(test sc_store)
target_link_libraries(test ${GLIB2_LIBRARIES})
