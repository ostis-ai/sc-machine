set(SC_BOTTLE_SRC "${SC_KPM_ROOT}/bottle")

set(SOURCES 
	"bottle.cpp"
       	"bottleKeynodes.cpp"
        "AOpenValveAgent.cpp"
        "AFillBottle.cpp"
        "AUnpressButtonAgent.cpp"
)
set(HEADERS
	"bottle.hpp"
       	"bottleKeynodes.hpp"
        "AOpenValveAgent.hpp"
        "AFillBottle.hpp"
        "AUnpressButtonAgent.hpp"
)

include_directories(${SC_BOTTLE_SRC} ${SC_MEMORY_SRC} ${GLIB2_INCLUDE_DIRS})

add_library (bottle SHARED ${SOURCES} ${HEADERS})
add_dependencies(bottle sc-memory-cpp sc-kpm-common)
target_link_libraries(bottle sc-memory-cpp sc-kpm-common)

sc_codegen(bottle ${SC_BOTTLE_SRC})

