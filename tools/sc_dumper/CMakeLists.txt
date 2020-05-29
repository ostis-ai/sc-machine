
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h" "src/*.hpp")

if (${WIN32})
    set (BOOST_LIBS_LIST )

elseif (${APPLE})
    set (BOOST_LIBS_LIST
            Boost::filesystem
            Boost::program_options
            Boost::system
            )
else()
    set (BOOST_LIBS_LIST
            boost_filesystem
            boost_program_options
            boost_system
            )
endif (${WIN32})

add_executable(sc_dumper ${SOURCES})
target_include_directories(sc_dumper
        PRIVATE ${SC_MEMORY_SRC}
        PRIVATE ${GLIB2_INCLUDE_DIRS}
        PRIVATE ${PYTHON_INCLUDE_DIRS}
        )
target_link_libraries(sc_dumper sc-core sc-memory ${BOOST_LIBS_LIST}
        ${LIBCURL_LIBRARIES}
        ${PYTHON_LIBRARIES}
        ${Boost_LIBRARIES})

add_dependencies(sc_dumper sc-code-generator sc-kpm sc-memory sc-core)

sc_codegen(sc_dumper ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/generated")


#todo ==============================================================================================

target_compile_definitions(sc_dumper PRIVATE "-DSC_MEMORY_SELF_BUILD")