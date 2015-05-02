set(SCTP_CLIENT_ROOT "${SC_MACHINE_ROOT}/sc-network/sctp_client")

file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")

add_library (sctp-client SHARED ${SOURCES} ${HEADERS})

target_link_libraries(sc-memory)
