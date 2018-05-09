set(SC_MERGE_SRC ${CMAKE_CURRENT_LIST_DIR})

file(GLOB_RECURSE SOURCES "*.c")
file(GLOB_RECURSE HEADERS "*.h")

add_library (sc-merge SHARED ${SOURCES} ${HEADERS})

include_directories(${SC_MERGE_SRC} ${SC_MEMORY_SRC} ${GLIB2_INCLUDE_DIRS})
add_dependencies(sc-merge sc-memory sc-kpm-common)
target_link_libraries(sc-merge sc-kpm-common)
