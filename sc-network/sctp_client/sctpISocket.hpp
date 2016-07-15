/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <cstdint>
#include <string>

namespace sctp
{

class ISocket
{
public:
    virtual ~ISocket() {}

    virtual bool connect(std::string const & host, std::string const & port) = 0;
    virtual void disconnect() = 0;

    virtual bool isConnected() const = 0;

    /** Reads data from socket into buffer (buffer size must be equal to bytesCount).
     * Returns number of bytes that was read. If returned value is -1,
     * then there was error while read data.
     */
    virtual int read(void * buffer, uint32_t bytesCount) = 0;

    /** Writes data into socket from buffer (buffer size must be equal to bytesCount)
     * Returns number of bytes that was written. If returned value is -1,
     * then there was error while write data.
     */
    virtual int write(void * buffer, uint32_t bytesCount) = 0;

    template <typename Type>
    int writeType(Type const & value)
    {
        return write((void*)&value, sizeof(Type));
    }

    template <typename Type>
    int readType(Type const & value)
    {
        return read((void*)&value, sizeof(Type));
    }
};

}
