set(SC_MEMORY_ROOT "${SC_MACHINE_ROOT}/sc-memory")
set(SC_STORE_ROOT "${SC_MACHINE_ROOT}/sc-memory/sc-store")

file(GLOB_RECURSE SOURCES "*.c")
file(GLOB_RECURSE HEADERS "*.h")

add_library (sc-memory SHARED ${SOURCES} ${HEADERS})

include_directories(${SC_STORE_ROOT} ${GLIB2_INCLUDE_DIRS})
target_link_libraries(sc-memory ${GLIB2_LIBRARIES})

# cpp wrapper
include_directories(${SC_MEMORY_SRC})

set (SC_MEMORY_WRAP_SOURCES 
			"wrap/sc_memory.cpp"
                        "wrap/sc_addr.cpp"
                        "wrap/sc_utils.cpp"
                        "wrap/sc_iterator.cpp"
                        "wrap/sc_stream.cpp"

                        "wrap/sc_memory.hpp"
                        "wrap/sc_addr.hpp"
                        "wrap/sc_utils.hpp"
                        "wrap/sc_iterator.hpp"
                        "wrap/sc_stream.hpp"
                        "wrap/sc_types.hpp"

        )

add_library (sc-memory-cpp SHARED ${SC_MEMORY_WRAP_SOURCES})

add_dependencies(sc-memory-cpp sc-memory)
target_link_libraries(sc-memory-cpp sc-memory)

target_compile_definitions(sc-memory PUBLIC "-DSC_MEMORY_SELF_BUILD")

# compile test
add_subdirectory(test)


