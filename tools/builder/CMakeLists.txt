file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")

file(GLOB_RECURSE SOURCES_C 
	"src/parser/*.c")

set (SC_BUILDER_DIR "${SC_TOOLS_DIR}/builder")

if (${UNIX})
	add_custom_command(OUTPUT "${SC_BUILDER_DIR}/src/parser/scsLexer.c" 
							"${SC_BUILDER_DIR}/src/parser/scsLexer.h" 
							"${SC_BUILDER_DIR}/src/parser/scsParser.c" 
							"${SC_BUILDER_DIR}/src/parser/scsParser.h"
			COMMAND "antlr3" ${SC_BUILDER_DIR}/grammar/scs.g
			-fo ${SC_BUILDER_DIR}/src/parser
			DEPENDS ${SC_BUILDER_DIR}/grammar/scs.g
	)

	add_custom_target(GenerateParser ALL
		DEPENDS ${SC_BUILDER_DIR}/src/parser
		SOURCES ${SC_BUILDER_DIR}/grammar/scs.g
	)

endif(${UNIX})

add_executable(sc-builder ${SOURCES} ${SOURCES_C} ${HEADERS} ${SC_BUILDER_DIR}/src/parser/scsLexer.c)
include_directories(${SC_MEMORY_SRC} ${GLIB2_INCLUDE_DIRS})
target_link_libraries(sc-builder sc-memory antlr3c boost_filesystem boost_program_options boost_system)
add_dependencies(sc-builder GenerateParser)

install_targets("/bin" sc-builder)
