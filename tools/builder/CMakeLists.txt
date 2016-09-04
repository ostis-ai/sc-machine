set (SC_BUILDER_DIR "${SC_TOOLS_DIR}/builder")

file(MAKE_DIRECTORY "${SC_BUILDER_DIR}/src/parser")


add_custom_command(OUTPUT "${SC_BUILDER_DIR}/src/parser/scsLexer.c" 
						"${SC_BUILDER_DIR}/src/parser/scsLexer.h" 
						"${SC_BUILDER_DIR}/src/parser/scsParser.c" 
						"${SC_BUILDER_DIR}/src/parser/scsParser.h"
		COMMAND ${JAVA_EXE} "-jar" "${SC_MACHINE_ANTRL_DIR}/antlr-3.2.jar" ${SC_BUILDER_DIR}/grammar/scs.g
		-fo ${SC_BUILDER_DIR}/src/parser
		DEPENDS ${SC_BUILDER_DIR}/grammar/scs.g
)

add_custom_target(GenerateParser ALL
	DEPENDS ${SC_BUILDER_DIR}/src/parser
	SOURCES ${SC_BUILDER_DIR}/grammar/scs.g
)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")

set (SOURCES_C 
	"${SC_BUILDER_DIR}/src/parser/scsLexer.c"
	"${SC_BUILDER_DIR}/src/parser/scsParser.c" )

if (${WIN32})
	set (BOOST_LIBS_LIST )
	set (ANTLR_LIB antlr3c.${MSVC_SUFFIX})
else()
 	set (BOOST_LIBS_LIST
 			boost_filesystem 
 			boost_program_options 
 			boost_system)
	set (ANTLR_LIB antlr3c)
endif (${WIN32})

	
add_executable(sc-builder ${SOURCES} ${SOURCES_C} ${HEADERS})
include_directories(${SC_MEMORY_SRC} ${GLIB2_INCLUDE_DIRS})
target_link_libraries(sc-builder sc-memory ${ANTLR_LIB} ${BOOST_LIBS_LIST})
add_dependencies(sc-builder GenerateParser)

install_targets("/bin" sc-builder)
