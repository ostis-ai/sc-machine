set(SC_IOT_SRC "${SC_KPM_ROOT}/iot")

set(SOURCES 
	"iot.cpp"
	"iotActions.cpp"
	"iotEnergy.cpp"
	"iotKeynodes.cpp"
	"iotCommands.cpp"
	"iotContent.cpp"
	"iotUtils.cpp"
	"iotAbout.cpp"
	"iotSpeech.cpp"
	"iotTV.cpp"
)
set(HEADERS 
	"iotTypes.hpp"
	"iot.hpp"
	"iotActions.hpp"
	"iotEnergy.hpp"
	"iotKeynodes.hpp"
	"iotCommands.hpp"
	"iotContent.hpp"
	"iotUtils.hpp"
	"iotAbout.hpp"
	"iotSpeech.hpp"
	"iotTV.hpp"
)

include_directories(${SC_IOT_SRC} ${SC_MEMORY_SRC} ${GLIB2_INCLUDE_DIRS})

add_library (iot SHARED ${SOURCES} ${HEADERS})
add_dependencies(iot sc-memory-cpp sc-kpm-common)
target_link_libraries(iot sc-memory-cpp sc-kpm-common)

sc_codegen(iot ${SC_IOT_SRC})
