set(SC_MACHINE_ANTRL_DIR "${SC_MACHINE_THIRDPARTY_PATH}/antlr")

add_custom_command(OUTPUT "${CMAKE_CURRENT_LIST_DIR}/scs/scsLexer.cpp"
                          "${CMAKE_CURRENT_LIST_DIR}/scs/scsLexer.h"
                          "${CMAKE_CURRENT_LIST_DIR}/scs/scsParser.cpp"
                          "${CMAKE_CURRENT_LIST_DIR}/scs/scsParser.h"
    COMMAND "${Java_JAVA_EXECUTABLE}" -jar "${SC_MACHINE_ANTRL_DIR}/antlr-4.7.1-complete.jar" scs.g4 -no-listener -package scs
    WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/scs"
    DEPENDS scs/scs.g4
)

add_custom_target(SCsParser ALL
      DEPENDS "${CMAKE_CURRENT_LIST_DIR}"
      SOURCES "scs/scs.g4"
)

set(SOURCES_PARSER
    "${CMAKE_CURRENT_LIST_DIR}/scs/scsLexer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/scs/scsLexer.h"
    "${CMAKE_CURRENT_LIST_DIR}/scs/scsParser.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/scs/scsParser.h"
)

file(GLOB_RECURSE SOURCES "*.cpp" "*.hpp")

add_library(sc-memory SHARED ${SOURCES} ${SOURCES_PARSER})

target_include_directories(sc-memory
    PRIVATE ${GLIB2_INCLUDE_DIRS}
    PRIVATE ${SC_MACHINE_ANTRL_DIR}/runtime/src
    PRIVATE ${SC_MEMORY_SRC}
)

if(${WIN32})
    target_link_libraries(sc-memory
        sc-core
        ${LIBCURL_LIBRARIES}
        antlr4_static
    )
elseif(${UNIX})
    target_link_libraries(sc-memory
        sc-core
        antlr4_static
        ${LIBCURL_LIBRARIES}
        ${Boost_LIBRARIES}
    )
endif(${WIN32})

sc_codegen_ex(sc-memory ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/generated")
add_dependencies(sc-memory 
    sc-core
    sc-code-generator
    antlr4_static
    SCsParser
)

target_compile_definitions(sc-memory PRIVATE "-DSC_MEMORY_SELF_BUILD")

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-memory)
endif()
