include_directories(${SC_MEMORY_SRC})

set (SC_MEMORY_CPP_ROOT "${SC_MEMORY_ROOT}/cpp")

add_custom_command(OUTPUT "${SC_MEMORY_CPP_ROOT}/scs/scsLexer.cpp" 
                          "${SC_MEMORY_CPP_ROOT}/scs/scsLexer.hpp" 
                          "${SC_MEMORY_CPP_ROOT}/scs/scsParser.cpp" 
                          "${SC_MEMORY_CPP_ROOT}/scs/scsParser.hpp"
      COMMAND ${JAVA_EXE} "-jar" "${SC_MACHINE_ANTRL_DIR}/antlr-3.5.2-complete.jar" ${SC_MEMORY_CPP_ROOT}/scs/scs.g
            -fo ${SC_MEMORY_CPP_ROOT}/scs/
            DEPENDS ${SC_MEMORY_CPP_ROOT}/scs/scs.g
)

add_custom_target(SCsParser ALL
      DEPENDS "${SC_MEMORY_CPP_ROOT}/scs/"
      SOURCES "${SC_MEMORY_CPP_ROOT}/scs/scs.g"
)

set (SOURCES_SCS 
      "${SC_MEMORY_CPP_ROOT}/scs/scsLexer.cpp"
      "${SC_MEMORY_CPP_ROOT}/scs/scsParser.cpp")

set (SC_MEMORY_CPP_SOURCES 
            "sc_memory.cpp"
            "sc_addr.cpp"
            "sc_event.cpp"
            "sc_debug.cpp"
            "sc_utils.cpp"
            "sc_iterator.cpp"
            "sc_keynodes.cpp"
            "sc_stream.cpp"
            "sc_template.cpp"
            "sc_template_build.cpp"
            "sc_template_gen.cpp"
            "sc_template_search.cpp"
            "sc_object.cpp"
            "sc_module.cpp"
            "sc_wait.cpp"
            "sc_struct.cpp"
            "sc_timer.cpp"
            "sc_types.cpp"
            "kpm/sc_agent.cpp"
            "http/sc_http.cpp"
            "http/sc_http_request.cpp"
            "http/sc_http_response.cpp"
            "python/sc_python_interp.cpp"
            "python/sc_python_module.cpp"
            "python/sc_python_gil.cpp"
            "utils/sc_log.cpp"
            "utils/sc_lock.cpp"
            "utils/sc_message.cpp"
            "utils/sc_console.cpp"
            "utils/sc_progress.cpp"
            "utils/sc_test.cpp"
            "scs/scs_parseutils.cpp"
            "scs/scs_parser.cpp"
            "scs/scs_types.cpp"

            "sc_memory.hpp"
            "sc_addr.hpp"
            "sc_event.hpp"
            "sc_utils.hpp"
            "sc_iterator.hpp"
            "sc_keynodes.hpp"
            "sc_stream.hpp"
            "sc_types.hpp"
            "sc_timer.hpp"
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
            "http/sc_http.hpp"
            "http/sc_http_request.hpp"
            "http/sc_http_response.hpp"
            "python/sc_python_interp.hpp"
            "python/sc_python_module.hpp"
            "python/sc_python_includes.hpp"
            "python/sc_python_gil.hpp"
            "utils/sc_cache.hpp"
            "utils/sc_log.hpp"
            "utils/sc_lock.hpp"
            "utils/sc_message.hpp"
            "utils/sc_console.hpp"
            "utils/sc_progress.hpp"
            "utils/sc_test.hpp"
            "scs/scs_parseutils.hpp"
            "scs/scs_parser.hpp"
            "scs/scs_types.hpp"
            "scs/scs_traits.hpp"
        )

include_directories(${PYTHON_INCLUDE_DIRS})
add_library (sc-memory-cpp SHARED ${SC_MEMORY_CPP_SOURCES} ${SOURCES_SCS})

if (${WIN32})
target_link_libraries(sc-memory-cpp
                        sc-memory ${ANTLR_LIB} ${LIBCURL_LIBRARIES} ${PYTHON_LIBRARY_RELEASE}
                        debug ${BOOST_PYTHON_LIBRARY_DEBUG}
                        optimized ${BOOST_PYTHON_LIBRARY_RELEASE})
elseif (${UNIX})
target_link_libraries(sc-memory-cpp
                      sc-memory
                      ${ANTLR_LIB}
                      ${LIBCURL_LIBRARIES}
                      ${PYTHON_LIBRARIES}
                      ${Boost_LIBRARIES})
endif(${WIN32})

sc_codegen_ex(sc-memory-cpp ${SC_MEMORY_CPP_ROOT} "${SC_MEMORY_CPP_ROOT}/generated")
add_dependencies(sc-memory-cpp sc-memory sc-code-generator SCsParser)

target_compile_definitions(sc-memory-cpp PRIVATE "-DSC_MEMORY_SELF_BUILD")
