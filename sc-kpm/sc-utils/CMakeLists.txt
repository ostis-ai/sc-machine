set(SC_UTILS_SRC ${CMAKE_CURRENT_LIST_DIR})

file(GLOB_RECURSE SOURCES "*.c")
file(GLOB_RECURSE HEADERS "*.h")

add_library (sc-utils SHARED ${SOURCES} ${HEADERS})

include_directories(${SC_UTILS_SRC} ${SC_MEMORY_SRC} ${GLIB2_INCLUDE_DIRS} ${REDIS_INCLUDE_DIRS})
add_dependencies(sc-utils sc-memory sc-kpm-common)
target_link_libraries(sc-utils sc-kpm-common ${REDIS_LIBRARIES})
