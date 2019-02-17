file(GLOB_RECURSE SOURCES "*.c" "*.h")

add_library (sc-fm-filesystem SHARED ${SOURCES})

target_include_directories(sc-fm-filesystem PRIVATE ${GLIB2_INCLUDE_DIRS})
target_link_libraries(sc-fm-filesystem sc-core ${GLIB2_LIBRARIES})
add_dependencies(sc-fm-filesystem sc-core)
