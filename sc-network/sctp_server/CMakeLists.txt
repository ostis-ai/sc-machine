
set (SOURCES
	"main.cpp"
	"sctpClient.cpp"
	"sctpCommand.cpp"
	"sctpServer.cpp"
	"sctpStatistic.cpp"
	"sctpEventManager.cpp"
	)
	
set (HEADERS
	"sctpClient.h"
	"sctpCommand.h"
	"sctpServer.h"
	"sctpStatistic.h"
	"sctpEventManager.h"
	)

add_executable(sctp-server ${SOURCES} ${HEADERS})

include_directories(${SC_MEMORY_SRC} ${GLIB2_INCLUDE_DIRS})

target_link_libraries(sctp-server sc-memory
                                  Qt5::Core
                                  Qt5::Network)

