file(GLOB_RECURSE SOURCES "*.c")
file(GLOB_RECURSE HEADERS "*.h")

add_library (sc_memory SHARED ${SOURCES} ${HEADERS})

include_directories(${sc_storage_SOURCE_DIR}/sc_store ${GLIB2_INCLUDE_DIRS})
target_link_libraries(sc_memory ${GLIB2_LIBRARIES})

