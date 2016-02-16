set(SOURCES 
	"iot.cpp"
	"iotEnergy.cpp"
	"iotKeynodes.cpp"
	"iotCommands.cpp"
	"iotContent.cpp"
	"iotUtils.cpp"
	"iotAgent.cpp"
	"iotAbout.cpp"
	"iotSpeech.cpp"
	"iotTV.cpp"
)
set(HEADERS 
	"iotTypes.hpp"
	"iot.hpp"
	"iotEnergy.hpp"
	"iotKeynodes.hpp"
	"iotCommands.hpp"
	"iotContent.hpp"
	"iotUtils.hpp"
	"iotAgent.hpp"
	"iotAbout.hpp"
	"iotSpeech.hpp"
	"iotTV.hpp"
)

add_library (iot SHARED ${SOURCES} ${HEADERS})


include_directories("${SC_KPM_ROOT}/iot" ${SC_MEMORY_SRC} ${GLIB2_INCLUDE_DIRS})
add_dependencies(iot sc-memory-cpp sc-kpm-common)
target_link_libraries(iot sc-memory-cpp sc-kpm-common)
