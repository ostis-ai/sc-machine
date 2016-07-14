
set (SOURCES
	"sc_keynodes.c"

	)

set (HEADERS
	"sc_keynodes.h"
	"sc_common_types.h"

	)

add_library(sc-kpm-common SHARED ${SOURCES} ${HEADERS})

include_directories("${SC_KPM_ROOT}/common" ${SC_MEMORY_SRC} ${GLIB2_INCLUDE_DIRS})
add_dependencies(sc-kpm-common sc-memory)
target_link_libraries(sc-kpm-common sc-memory ${GLIB2_LIBRARIES})
target_compile_definitions(sc-kpm-common PUBLIC "-DSC_KPM_COMMON_SELF_BUILD")

install_targets("/lib/sc-memory/extensions" sc-kpm-common)
