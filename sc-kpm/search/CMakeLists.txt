file(GLOB_RECURSE SOURCES "*.c")
file(GLOB_RECURSE HEADERS "*.h")

add_library (search SHARED ${SOURCES} ${HEADERS})


include_directories("${SC_KPM_ROOT}/search" ${SC_MEMORY_SRC} ${GLIB2_INCLUDE_DIRS})
add_dependencies(search sc_memory)


