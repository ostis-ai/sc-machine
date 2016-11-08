/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "../sctpISocket.hpp"
#include "sc-memory/sc-store/sc_defines.h"

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

namespace sctp
{

class winSocket : public ISocket
{

public:
	_SC_EXTERN explicit winSocket();

	_SC_EXTERN bool connect(std::string const & host, std::string const & port);
	_SC_EXTERN void disconnect();

	_SC_EXTERN bool isConnected() const;

    /** Reads data from socket into buffer (buffer size must be equal to bytesCount).
     * Returns number of bytes that was read. If returned value is -1,
     * then there was error while read data.
     */
	_SC_EXTERN int read(void * buffer, unsigned int bytesCount);

    /** Writes data into socket from buffer (buffer size must be equal to bytesCount)
     * Returns number of bytes that was written. If returned value is -1,
     * then there was error while write data.
     */
	_SC_EXTERN int write(void * buffer, unsigned int bytesCount);

	_SC_EXTERN static bool initialize();
	_SC_EXTERN static void shutdown();

private:
    SOCKET mSocket;
};

}
