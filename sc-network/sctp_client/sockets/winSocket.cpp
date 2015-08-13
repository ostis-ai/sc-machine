/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "winSocket.hpp"
#include <assert.h>
#include <stdio.h>

namespace sctp
{

winSocket::winSocket()
    : mSocket(INVALID_SOCKET)
{

}

bool winSocket::connect(std::string const & host, std::string const & port)
{
    int portValue = atoi(port.c_str());
    assert(portValue > 0);

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(portValue);
    address.sin_addr.s_addr = inet_addr(host.c_str());//InetPton(address.sin_family, host.c_str(), &address.sin_addr.s_addr);

    mSocket = socket(address.sin_family, SOCK_STREAM, 0);
    if (mSocket == INVALID_SOCKET)
    {
        printf("Could not create socket: %d", WSAGetLastError());
        return false;
    }

    if (::connect(mSocket, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        printf("Can't connect to server: %s:%s", host.c_str(), port.c_str());
        disconnect();
        return false;
    }


    return true;
}

void winSocket::disconnect()
{
    closesocket(mSocket);
    mSocket = INVALID_SOCKET;
}

bool winSocket::isConnected() const
{
    return (mSocket != INVALID_SOCKET);
}

int winSocket::read(void * buffer, unsigned int bytesCount)
{
    unsigned int bytesRead = 0;
    char * buff = (char*)buffer;

    while (bytesRead < bytesCount)
    {
        int bytes = ::recv(mSocket, (&buff[bytesRead]), bytesCount - bytesRead, 0);
        if (bytes == SOCKET_ERROR)
            return -1;

        bytesRead += bytes;
    }

    assert(bytesRead == bytesCount);
    return bytesRead;
}

int winSocket::write(void * buffer, unsigned int bytesCount)
{
    unsigned int bytesSent = 0;
    char const * buff = (char*)buffer;

    while (bytesSent < bytesCount)
    {
        int bytes = ::send(mSocket, (&(buff[bytesSent])), bytesCount - bytesSent, 0);
        if (bytes == SOCKET_ERROR)
            return -1;

        bytesSent += bytes;
    }

    assert(bytesSent == bytesCount);

    return bytesSent;
}


bool winSocket::initialize()
{
    WSADATA wsaData;
    return (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0);
}

void winSocket::shutdown()
{
    WSACleanup();
}

}
