file(GLOB_RECURSE SOURCES_C "*.c")
file(GLOB_RECURSE SOURCES_CPP "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")

add_library (scp_lib STATIC ${SOURCES_CPP} ${SOURCES_C} ${HEADERS})

include_directories("${SCP_MACHINE_ROOT}/scp_lib" ${SC_MEMORY_SRC} ${GLIB2_INCLUDE_DIRS})
add_dependencies(scp_lib sc-memory)

#install_targets("/lib/sc-memory" scp_lib)
