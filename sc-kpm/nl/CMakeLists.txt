set(SC_IOT_SRC "${SC_KPM_ROOT}/nl")

set(SOURCES 
	"nl.cpp"
	"nlApiAi.cpp"
	"nlApiAiUtils.cpp"
	"nlDialogue.cpp"
	"nlGenText.cpp"
	"nlGenCmdTextResult.cpp"
	"nlIvona.cpp"
	"nlKeynodes.cpp"
	"nlTextTemplProcessor.cpp"
	
	"hash/sha256.cpp"
)
set(HEADERS 
	"nl.hpp"
	"nlApiAi.hpp"
	"nlApiAiUtils.hpp"
	"nlDialogue.hpp"
	"nlGenText.hpp"
	"nlGenCmdTextResult.hpp"
	"nlIvona.hpp"
	"nlKeynodes.hpp"
	"nlTextTemplProcessor.hpp"
	"nlTextTemplProcessorTokens.hpp"
)

include_directories(${SC_IOT_SRC} ${SC_MEMORY_SRC} ${GLIB2_INCLUDE_DIRS} ${LIBCURL_INCLUDE_DIRS})

add_library (nl SHARED ${SOURCES} ${HEADERS})
add_dependencies(nl sc-memory-cpp sc-kpm-common)
target_link_libraries(nl sc-memory-cpp sc-kpm-common ${LIBCURL_LIBRARIES})

sc_codegen(nl ${SC_IOT_SRC})
