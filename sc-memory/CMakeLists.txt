set(SC_MEMORY_ROOT "${SC_MACHINE_ROOT}/sc-memory")
set(SC_STORE_ROOT "${SC_MACHINE_ROOT}/sc-memory/src/sc-store")

file(GLOB_RECURSE SOURCES "src/*.c")
file(GLOB_RECURSE HEADERS "src/*.h")

add_library (sc_memory SHARED ${SOURCES} ${HEADERS})

include_directories(${sc_storage_SOURCE_DIR}/sc_store ${GLIB2_INCLUDE_DIRS})
target_link_libraries(sc_memory ${GLIB2_LIBRARIES})

# compile test
add_subdirectory(test)


