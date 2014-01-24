file(GLOB_RECURSE SOURCES "*.c")
file(GLOB_RECURSE HEADERS "*.h")

add_library (utils SHARED ${SOURCES} ${HEADERS})


include_directories("${SC_KPM_ROOT}/utils" ${SC_MEMORY_SRC} ${GLIB2_INCLUDE_DIRS})
add_dependencies(utils sc-memory)
target_link_libraries(utils ${REDIS_LIBRARIES})

install_targets("/lib/sc-memory/extensions" utils)


