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

  virtual bool Connect(std::string const & host, std::string const & port) = 0;
  virtual void Disconnect() = 0;

  virtual bool IsConnected() const = 0;

  /** Reads data from socket into buffer (buffer size must be equal to bytesCount).
   * Returns number of bytes that was read. If returned value is -1,
   * then there was error while read data.
   */
  virtual int Read(void * buffer, uint32_t bytesCount) = 0;

  /** Writes data into socket from buffer (buffer size must be equal to bytesCount)
   * Returns number of bytes that was written. If returned value is -1,
   * then there was error while write data.
   */
  virtual int Write(void * buffer, uint32_t bytesCount) = 0;

  template <typename Type>
  int WriteType(Type const & value)
  {
    return Write((void*)&value, sizeof(Type));
  }

  template <typename Type>
  int ReadType(Type const & value)
  {
    return Read((void*)&value, sizeof(Type));
  }
};

}
