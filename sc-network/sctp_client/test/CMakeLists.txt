
set (SOCKET_SRC
    "../sockets/winSocket.cpp"
    "../sockets/winSocket.hpp"
)


add_executable(sctp-client-test test_client.cpp ${SOCKET_SRC})
target_link_libraries(sctp-client-test sctp-client ${GLIB2_LIBRARIES})

