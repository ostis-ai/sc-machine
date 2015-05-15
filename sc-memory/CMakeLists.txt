set(SC_MEMORY_ROOT "${SC_MACHINE_ROOT}/sc-memory")
set(SC_STORE_ROOT "${SC_MACHINE_ROOT}/sc-memory/sc-store")

file(GLOB_RECURSE SOURCES "*.c")
file(GLOB_RECURSE HEADERS "*.h")

add_library (sc-memory SHARED ${SOURCES} ${HEADERS})

include_directories(${SC_STORE_ROOT} ${GLIB2_INCLUDE_DIRS})
target_link_libraries(sc-memory ${GLIB2_LIBRARIES})

# cpp wrapper
add_library (sc-memory-cpp SHARED "wrap/sc_memory.cpp" "wrap/sc_memory.hpp")
add_dependencies(sc-memory-cpp sc-memory)
target_link_libraries(sc-memory-cpp sc-memory)


# compile test
add_subdirectory(test)


