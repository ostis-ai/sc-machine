file(GLOB_RECURSE SOURCES "*.c")
file(GLOB_RECURSE HEADERS "*.h")

add_library (sc-fm-filesystem SHARED ${SOURCES} ${HEADERS})

include_directories("${SC_MEMORY_SRC}/sc-store" ${GLIB2_INCLUDE_DIRS})
target_link_libraries(sc-fm-filesystem sc-memory ${GLIB2_LIBRARIES})
add_dependencies(sc-fm-filesystem sc-memory)
