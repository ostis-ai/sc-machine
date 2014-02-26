set(SC_MEMORY_ROOT "${SC_MACHINE_ROOT}/sc-memory")
set(SC_STORE_ROOT "${SC_MACHINE_ROOT}/sc-memory/src/sc-store")

file(GLOB_RECURSE SOURCES "src/*.c")
file(GLOB_RECURSE HEADERS "src/*.h")

add_library (sc-memory SHARED ${SOURCES} ${HEADERS})

include_directories(${SC_STORE_ROOT} ${GLIB2_INCLUDE_DIRS})
target_link_libraries(sc-memory ${GLIB2_LIBRARIES})

# install
install_targets("/lib" sc-memory)

# compile test
add_subdirectory(test)


