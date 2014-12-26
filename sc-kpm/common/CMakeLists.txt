file(GLOB_RECURSE SOURCES "*.c")
file(GLOB_RECURSE HEADERS "*.h")

add_library(sc-kpm-common SHARED ${SOURCES} ${HEADERS})

include_directories("${SC_KPM_ROOT}/common" ${SC_MEMORY_SRC} ${GLIB2_INCLUDE_DIRS})
add_dependencies(sc-kpm-common sc-memory)
target_link_libraries(sc-kpm-common ${GLIB2_LIBRARIES})

install_targets("/lib/sc-memory/extensions" sc-kpm-common)
