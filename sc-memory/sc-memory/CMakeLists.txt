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

file(GLOB SOURCES CONFIGURE_DEPENDS
    "*.cpp" "*.tpp" "*.hpp"
    "*/*.cpp" "*/*.hpp"
)

add_library(sc-memory SHARED ${SOURCES} ${SOURCES_PARSER})
target_link_libraries(sc-memory
    LINK_PUBLIC sc-core
    LINK_PRIVATE antlr4_static
)
target_include_directories(sc-memory
    PUBLIC ${SC_MEMORY_SRC}
    PRIVATE ${SC_MACHINE_ANTRL_DIR}/runtime/src
)

add_dependencies(sc-memory 
    sc-core
    antlr4_static
    SCsParser
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(SCsParser)
endif()

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-memory)
endif()
