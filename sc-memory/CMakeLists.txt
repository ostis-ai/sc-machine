set(SC_MEMORY_ROOT "${SC_MACHINE_ROOT}/sc-memory")
set(SC_STORE_ROOT "${SC_MACHINE_ROOT}/sc-memory/sc-store")

file(GLOB_RECURSE SOURCES "*.c")
file(GLOB_RECURSE HEADERS "*.h")

add_library (sc-memory SHARED ${SOURCES} ${HEADERS})

include_directories(${GLIB2_INCLUDE_DIRS})
target_link_libraries(sc-memory ${GLIB2_LIBRARIES})


add_subdirectory(cpp)
add_subdirectory(test)
