include_directories(${SC_MEMORY_SRC})

set (SC_MEMORY_CPP_ROOT "${SC_MEMORY_ROOT}/cpp")

set (SC_MEMORY_CPP_SOURCES 
            "sc_memory.cpp"
            "sc_addr.cpp"
            "sc_event.cpp"
            "sc_utils.cpp"
            "sc_iterator.cpp"
            "sc_stream.cpp"
            "sc_template.cpp"
            "sc_template_build.cpp"
            "sc_template_gen.cpp"
            "sc_template_search.cpp"
            "sc_object.cpp"
            "sc_module.cpp"
            "sc_wait.cpp"
            "sc_struct.cpp"
            "sc_types.cpp"
            "kpm/sc_agent.cpp"
            "utils/sc_log.cpp"
            "utils/sc_message.cpp"

            "sc_memory.hpp"
            "sc_addr.hpp"
            "sc_event.hpp"
            "sc_utils.hpp"
            "sc_iterator.hpp"
            "sc_stream.hpp"
            "sc_types.hpp"
            "sc_template.hpp"
            "sc_object.hpp"
            "sc_defines.hpp"
            "sc_module.hpp"
            "sc_wait.hpp"
            "sc_struct.hpp"
            "sc_platform.hpp"
            "sc_memory_headers.hpp"
            "sc_debug.hpp"
            "kpm/sc_agent.hpp"
            "utils/sc_log.hpp"
            "utils/sc_message.hpp"
        )

add_library (sc-memory-cpp SHARED ${SC_MEMORY_CPP_SOURCES})

add_dependencies(sc-memory-cpp sc-memory sc-code-generator)
target_link_libraries(sc-memory-cpp sc-memory)
sc_codegen_ex(sc-memory-cpp ${SC_MEMORY_CPP_ROOT} "${SC_MEMORY_CPP_ROOT}/generated")

target_compile_definitions(sc-memory PRIVATE "-DSC_MEMORY_SELF_BUILD")
target_compile_definitions(sc-memory-cpp PRIVATE "-DSC_MEMORY_SELF_BUILD")