set(SCTP_CLIENT_ROOT "${SC_MACHINE_ROOT}/sc-network/sctp")

file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")

add_library (sctp STATIC ${SOURCES} ${HEADERS})

target_link_libraries(sc-memory)
