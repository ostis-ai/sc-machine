/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once
#include <string>

extern "C"
{
#include <glib.h>
#include <gio/gio.h>
}

#include "../sctpISocket.hpp"

namespace sctp
{

class glibSocket : public ISocket
{

public:
    explicit glibSocket();

    bool connect(std::string const & host, std::string const & port);
    void disconnect();

    bool isConnected() const;

    /** Reads data from socket into buffer (buffer size must be equal to bytesCount).
     * Returns number of bytes that was read. If returned value is -1,
     * then there was error while read data.
     */
    int read(void * buffer, unsigned int bytesCount);

    /** Writes data into socket from buffer (buffer size must be equal to bytesCount)
     * Returns number of bytes that was written. If returned value is -1,
     * then there was error while write data.
     */
    int write(void * buffer, unsigned int bytesCount);

private:
    GSocketConnection * mConnection;
    GSocketClient * mClient;

    GInputStream * mInputStream;
    GOutputStream * mOutputStream;
};

}
