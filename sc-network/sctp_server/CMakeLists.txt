
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

target_include_directories(sctp-server
	PRIVATE ${SC_MEMORY_SRC} 
	PRIVATE ${GLIB2_INCLUDE_DIRS}
	)

target_link_libraries(sctp-server sc-memory sc-memory-cpp
                                  Qt5::Core
                                  Qt5::Network)

add_dependencies(sctp-server
	sc-kpm
	)