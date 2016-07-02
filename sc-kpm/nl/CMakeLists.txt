set(SC_IOT_SRC "${SC_KPM_ROOT}/nl")

set(SOURCES 
	"nl.cpp"
	"nlApiAi.cpp"
)
set(HEADERS 
	"nl.hpp"
	"nlApiAi.hpp"
	"nlApiAiUtils.hpp"
)

include_directories(${SC_IOT_SRC} ${SC_MEMORY_SRC} ${GLIB2_INCLUDE_DIRS} ${CURL_INCLUDE})

add_library (nl SHARED ${SOURCES} ${HEADERS})
add_dependencies(nl sc-memory-cpp sc-kpm-common)
target_link_libraries(nl sc-memory-cpp sc-kpm-common ${CURL_LIBRARIES})

sc_codegen(nl ${SC_IOT_SRC})
