set(SCTP_CLIENT_ROOT "${SC_MACHINE_ROOT}/sc-network/sctp_client")

set (SOURCES
        "sctpClient.cpp")

set (HEADERS
        "sctpClient.hpp"
        "sctpTypes.hpp"
        "sctpISocket.hpp"
        )

add_library (sctp-client SHARED ${SOURCES} ${HEADERS})
target_link_libraries(sctp-client sc-memory-cpp)
target_compile_definitions(sctp-client PUBLIC "-DSC_MEMORY_SELF_BUILD")

add_subdirectory(test)
