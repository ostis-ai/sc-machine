set(SCTP_CLIENT_ROOT "${SC_MACHINE_ROOT}/sc-network/sctp_client")

set (SOURCES
        "sctpClient.cpp")

set (HEADERS
        "sctpClient.hpp"
        "sctpTypes.hpp"
        "sctpISocket.hpp"
        )

if (${WIN32})
    set (SOCKET_SRC
        "sockets/winSocket.cpp"
        "sockets/winSocket.hpp"
    )
    set (SOCKET_LIBS
    )
else ()
    set (SOCKET_SRC
        "sockets/glibSocket.cpp"
        "sockets/glibSocket.hpp"
    )
    set (SOCKET_LIBS  ${GLIB2_LIBRARIES})
endif()

add_library (sctp-client SHARED ${SOURCES} ${HEADERS} ${SOCKET_SRC})
target_link_libraries(sctp-client sc-memory-cpp ${SOCKET_LIBS})
target_compile_definitions(sctp-client PUBLIC "-DSC_MEMORY_SELF_BUILD")

if (${WIN32})
add_subdirectory(test)
endif ()
