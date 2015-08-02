
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
	"sctpTypes.h"
	)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)

add_executable(sctp-server ${SOURCES} ${HEADERS})

include_directories(${SC_MEMORY_SRC} 
					${GLIB2_INCLUDE_DIRS})

target_link_libraries(sctp-server sc-memory
					Qt5::Core
					Qt5::Network)

install_targets("/bin" sctp-server)
