set (SC_CODEGEN_DIR "${SC_TOOLS_DIR}/codegen")


set (SOURCES 
	"Parser/LanguageTypes/Class.cpp"
	"Parser/LanguageTypes/Constructor.cpp"
	"Parser/LanguageTypes/Enum.cpp"
	"Parser/LanguageTypes/Field.cpp"
	"Parser/LanguageTypes/Function.cpp"
	"Parser/LanguageTypes/Global.cpp"
	"Parser/LanguageTypes/Invokable.cpp"
	"Parser/LanguageTypes/LanguageType.cpp"
	"Parser/LanguageTypes/Method.cpp"

	"Parser/Main.cpp"
	"Parser/Cursor.cpp"
	"Parser/CursorType.cpp"
	"Parser/MetaUtils.cpp"
	"Parser/MetaDataManager.cpp"
	"Parser/Precompiled.cpp"
	"Parser/ReflectionParser.cpp"
	"Parser/MacrosManager.cpp"
)

set (HEADERS
	"Parser/LanguageTypes/Class.hpp"
	"Parser/LanguageTypes/Constructor.hpp"
	"Parser/LanguageTypes/Enum.hpp"
	"Parser/LanguageTypes/Field.hpp"
	"Parser/LanguageTypes/Function.hpp"
	"Parser/LanguageTypes/Global.hpp"
	"Parser/LanguageTypes/Invokable.hpp"
	"Parser/LanguageTypes/LanguageType.hpp"
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
	"Parser/Templates.hpp"
	"Parser/MacrosManager.hpp"
	"Parser/Types.hpp"
)


link_directories(${SC_MACHINE_THIRDPARTY_PATH}/clang/lib)

add_executable(sc-code-generator ${SOURCES} ${HEADERS})
include_directories(${SC_CODEGEN_DIR}/Parser ${SC_MACHINE_THIRDPARTY_PATH}/clang/include)
target_link_libraries(sc-code-generator libclang)
# add_dependencies(sc-builder)

install_targets("/bin" sc-code-generator)
