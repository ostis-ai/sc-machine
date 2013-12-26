
set (SOURCES
	"main.cpp"
	"sctpClient.cpp"
	"sctpCommand.cpp"
	"sctpServer.cpp"
	"sctpStatistic.cpp"
	)
	
set (HEADERS
	"sctpClient.h"
	"sctpCommand.h"
	"sctpServer.h"
	"sctpStatistic.h"
	"sctpTypes.h"
	)

if (${UNIX})

	find_package (Qt4 COMPONENTS QtCore QtMain QtNetwork REQUIRED)
	include(${QT_USE_FILE})
	
	QT4_WRAP_CPP(HEADERS_MOC ${HEADERS})
	add_definitions(${QT_DEFINITIONS})

endif(${UNIX})

add_executable(sctp-server ${SOURCES} ${HEADERS} ${HEADERS_MOC})
include_directories(${SC_MEMORY_SRC} ${GLIB2_INCLUDE_DIRS} ${Qt4_INCLUDE_DIRS})
target_link_libraries(sctp-server sc-memory ${QT_LIBRARIES})

install_targets("/bin" sctp-server)
