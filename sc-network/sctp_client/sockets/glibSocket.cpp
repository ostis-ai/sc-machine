/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "glibSocket.hpp"
#include <stdlib.h>

namespace sctp
{

glibSocket::glibSocket()
    : mConnection(NULL)
    , mClient(NULL)
{

}

bool glibSocket::connect(std::string const & host, std::string const & port)
{
    mClient = g_socket_client_new();
    g_assert(mClient);

    std::string hostAndPort = host + ":" + port;
    gint portValue = atoi(port.c_str());

    GError * error = NULL;
    g_assert(portValue > 0);
    mConnection = g_socket_client_connect_to_host(mClient, hostAndPort.c_str(), (guint16)portValue, NULL, &error);

    if (error != NULL)
    {
        g_error_free(error);
        disconnect();
        return false;
    }
    else
        g_print("Connected to %s", hostAndPort.c_str());

    mInputStream = g_io_stream_get_input_stream(G_IO_STREAM(mConnection));
    mOutputStream = g_io_stream_get_output_stream(G_IO_STREAM(mConnection));

    g_assert(mInputStream);
    g_assert(mOutputStream);

    return true;
}

void glibSocket::disconnect()
{
    if (mClient)
    {
        g_object_unref(mClient);
        mClient = NULL;
    }

    if (mConnection)
    {
        g_object_unref(mConnection);
        mConnection = NULL;
    }
}

bool glibSocket::isConnected() const
{
    return mConnection;
}

int glibSocket::read(void * buffer, unsigned int bytesCount)
{
    g_assert(mInputStream);
    gsize readBytes = 0;

    if (g_input_stream_read_all(mInputStream, buffer, (gsize)bytesCount, &readBytes, NULL, NULL) == FALSE)
        return -1;

    return (int)readBytes;
}

int glibSocket::write(void * buffer, unsigned int bytesCount)
{
    g_assert(mOutputStream);
    gsize writtenBytes = 0;
    if (g_output_stream_write_all(mOutputStream, buffer, bytesCount, &writtenBytes, NULL, NULL) == FALSE)
        return -1;

    return (int)writtenBytes;
}


}
