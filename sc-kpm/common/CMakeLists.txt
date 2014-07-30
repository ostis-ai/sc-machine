file(GLOB_RECURSE SOURCES "*.c")
file(GLOB_RECURSE HEADERS "*.h")

file(GLOB_RECURSE SOURCES_PRIV "private/*.c")
file(GLOB_RECURSE HEADERS_PRIV "private/*.h")

add_library (sc-kpm-common SHARED ${SOURCES} ${HEADERS} ${SOURCES_PRIV} ${HEADERS_PRIV})


include_directories("${SC_KPM_ROOT}/common" ${SC_MEMORY_SRC} ${GLIB2_INCLUDE_DIRS})
target_link_libraries(sc-kpm-common ${GLIB2_LIBRARIES})
add_dependencies(sc-kpm-common sc-memory)

install_targets("/lib/sc-memory/extensions" sc-kpm-common)
