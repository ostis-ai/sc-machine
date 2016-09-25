set(SC_IOT_SRC "${SC_KPM_ROOT}/nl")

set(SOURCES 
	"nl.cpp"
	"nlApiAi.cpp"
	"nlDialogue.cpp"
	"nlIvona.cpp"
	"nlKeynodes.cpp"
	"hash/sha256.cpp"
)
set(HEADERS 
	"nl.hpp"
	"nlApiAi.hpp"
	"nlApiAiUtils.hpp"
	"nlDialogue.hpp"
	"nlIvona.hpp"
	"nlKeynodes.hpp"
)

include_directories(${SC_IOT_SRC} ${SC_MEMORY_SRC} ${GLIB2_INCLUDE_DIRS} ${LIBCURL_INCLUDE_DIRS})

add_library (nl SHARED ${SOURCES} ${HEADERS})
add_dependencies(nl sc-memory-cpp sc-kpm-common)
target_link_libraries(nl sc-memory-cpp sc-kpm-common ${LIBCURL_LIBRARIES})

sc_codegen(nl ${SC_IOT_SRC})
