set (SC_MACHINE_ANTRL_DIR "${SC_MACHINE_THIRDPARTY_PATH}/antlr")

add_custom_command(OUTPUT "${CMAKE_CURRENT_LIST_DIR}/scsLexer.cpp"
                          "${CMAKE_CURRENT_LIST_DIR}/scsLexer.h"
                          "${CMAKE_CURRENT_LIST_DIR}/scsParser.cpp"
                          "${CMAKE_CURRENT_LIST_DIR}/scsParser.h"
      COMMAND "${JAVA_EXE}" -jar "${SC_MACHINE_ANTRL_DIR}/antlr-4.7.1-complete.jar" scs.g4 -no-listener -package scs
      WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
      DEPENDS scs.g4
)

add_custom_target(SCsParser ALL
      DEPENDS "${CMAKE_CURRENT_LIST_DIR}"
      SOURCES "scs.g4"
)

set (SOURCES_PARSER
      "${CMAKE_CURRENT_LIST_DIR}/scsLexer.cpp"
      "${CMAKE_CURRENT_LIST_DIR}/scsLexer.h"
      "${CMAKE_CURRENT_LIST_DIR}/scsParser.cpp"
      "${CMAKE_CURRENT_LIST_DIR}/scsParser.h"
      )

set (SOURCES_SCS
      
      "scs_parser.cpp"
      "scs_types.cpp"

      "scs_parser.hpp"
      "scs_traits.hpp"
      "scs_types.hpp"
      )

add_library(scs-parser ${SOURCES_SCS} ${SOURCES_PARSER})
target_include_directories(scs-parser PRIVATE ${SC_MACHINE_ANTRL_DIR}/runtime/src)
target_link_libraries(scs-parser antlr4_shared)
add_dependencies(scs-parser antlr4_shared SCsParser)