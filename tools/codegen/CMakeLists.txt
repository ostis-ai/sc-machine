set (SC_CODEGEN_DIR "${SC_TOOLS_DIR}/codegen")


set (SOURCES 
	"Parser/LanguageTypes/Class.cpp"
	"Parser/LanguageTypes/Constructor.cpp"
	"Parser/LanguageTypes/Field.cpp"
	"Parser/LanguageTypes/Global.cpp"
	"Parser/LanguageTypes/LanguageType.cpp"
    "Parser/LanguageTypes/Invokable.cpp"
    "Parser/LanguageTypes/Function.cpp"
    "Parser/LanguageTypes/Method.cpp"

	"Parser/Main.cpp"
	"Parser/Cursor.cpp"
	"Parser/CursorType.cpp"
	"Parser/MetaUtils.cpp"
	"Parser/MetaDataManager.cpp"
	"Parser/Precompiled.cpp"
	"Parser/ReflectionParser.cpp"
	"Parser/MacrosManager.cpp"
	"Parser/Cache.cpp"
	"Parser/Sha256.cpp"
)

set (HEADERS
	"Parser/LanguageTypes/Class.hpp"
	"Parser/LanguageTypes/Constructor.hpp"
	"Parser/LanguageTypes/Field.hpp"
	"Parser/LanguageTypes/Global.hpp"
	"Parser/LanguageTypes/LanguageType.hpp"
    "Parser/LanguageTypes/Invokable.hpp"
    "Parser/LanguageTypes/Function.hpp"
    "Parser/LanguageTypes/Method.hpp"

	"Parser/MetaDataConfig.hpp"
	"Parser/Cursor.hpp"
	"Parser/CursorType.hpp"
	"Parser/MetaUtils.hpp"
	"Parser/MetaDataManager.hpp"
	"Parser/Namespace.hpp"
	"Parser/Precompiled.hpp"
	"Parser/ReflectionOptions.hpp"
	"Parser/ReflectionParser.hpp"
	"Parser/ReservedTypes.hpp"
	"Parser/Switches.hpp"
	"Parser/MacrosManager.hpp"
	"Parser/Types.hpp"
	"Parser/Cache.hpp"
	"Parser/Sha256.hpp"
)

add_executable(sc-code-generator ${SOURCES} ${HEADERS})
include_directories(${SC_CODEGEN_DIR}/Parser ${LIBCLANG_INCLUDE_DIRS})

if (${WIN32})
    set (BOOST_LIBS_LIST )
else()
    set (BOOST_LIBS_LIST
            boost_filesystem
            boost_program_options
            boost_system
            boost_regex)

    set_target_properties(sc-code-generator PROPERTIES COMPILE_FLAGS ${LIBCLANG_CXXFLAGS})
endif (${WIN32})

target_link_libraries(sc-code-generator ${LIBCLANG_LIBRARY} ${BOOST_LIBS_LIST})

install_targets("/bin" sc-code-generator)
