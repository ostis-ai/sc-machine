set(SC_MEMORY_ROOT "${SC_MACHINE_ROOT}/sc-memory")
set(SC_STORE_ROOT "${SC_MACHINE_ROOT}/sc-memory/sc-store")

file(GLOB_RECURSE SOURCES "*.c")
file(GLOB_RECURSE HEADERS "*.h")

add_library (sc-memory SHARED ${SOURCES} ${HEADERS})

include_directories(${SC_STORE_ROOT} ${GLIB2_INCLUDE_DIRS})
target_link_libraries(sc-memory ${GLIB2_LIBRARIES})

# cpp wrapper
include_directories(${SC_MEMORY_SRC})

set (SC_MEMORY_WRAP_ROOT "${SC_MEMORY_ROOT}/wrap")

set (SC_MEMORY_WRAP_SOURCES 
			"wrap/sc_memory.cpp"
            "wrap/sc_addr.cpp"
            "wrap/sc_event.cpp"
            "wrap/sc_utils.cpp"
            "wrap/sc_iterator.cpp"
            "wrap/sc_stream.cpp"
			"wrap/sc_template.cpp"
			"wrap/sc_template_build.cpp"
			"wrap/sc_template_gen.cpp"
			"wrap/sc_template_search.cpp"
			"wrap/sc_object.cpp"
            "wrap/sc_module.cpp"
            "wrap/sc_wait.cpp"
			"wrap/sc_struct.cpp"
			"wrap/sc_types.cpp"
			"wrap/kpm/sc_agent.cpp"

            "wrap/sc_memory.hpp"
            "wrap/sc_addr.hpp"
            "wrap/sc_event.hpp"
            "wrap/sc_utils.hpp"
            "wrap/sc_iterator.hpp"
            "wrap/sc_stream.hpp"
            "wrap/sc_types.hpp"
            "wrap/sc_template.hpp"
            "wrap/sc_object.hpp"
            "wrap/sc_defines.hpp"
            "wrap/sc_module.hpp"
            "wrap/sc_wait.hpp"
			"wrap/sc_struct.hpp"
			"wrap/sc_platform.hpp"
			"wrap/sc_memory_headers.hpp"
			"wrap/kpm/sc_agent.hpp"
        )

add_library (sc-memory-cpp SHARED ${SC_MEMORY_WRAP_SOURCES})

add_dependencies(sc-memory-cpp sc-memory sc-code-generator)
target_link_libraries(sc-memory-cpp sc-memory)
sc_codegen_ex(sc-memory-cpp ${SC_MEMORY_WRAP_ROOT} "${SC_MEMORY_WRAP_ROOT}/generated")

target_compile_definitions(sc-memory PRIVATE "-DSC_MEMORY_SELF_BUILD")
target_compile_definitions(sc-memory-cpp PRIVATE "-DSC_MEMORY_SELF_BUILD")

# compile test
add_subdirectory(test)


