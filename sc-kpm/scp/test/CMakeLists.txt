add_executable(scp_test main.c)
include_directories("${SCP_MACHINE_ROOT}/scp_lib/src" ${SC_MEMORY_SRC} ${GLIB2_INCLUDE_DIRS})

target_link_libraries(scp_test scp_lib sc-memory ${GLIB2_LIBRARIES})
