/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

extern "C"
{
#include "sc-core/sc_memory_headers.h"
}

#include "sc_utils.hpp"

#include <type_traits>

class ScStream
{
  friend class ScMemoryContext;

  ScStream(ScStream const & other) = delete;
  ScStream & operator=(ScStream const & other) = delete;

public:
  _SC_EXTERN explicit ScStream();
  _SC_EXTERN explicit ScStream(sc_stream * stream);
  _SC_EXTERN explicit ScStream(std::string const & fileName, sc_uint8 flags);
  _SC_EXTERN explicit ScStream(sc_char * buffer, size_t bufferSize, sc_uint8 flags);
  _SC_EXTERN explicit ScStream(sc_char const * buffer, size_t bufferSize, sc_uint8 flags);

  _SC_EXTERN ~ScStream();

  _SC_EXTERN void Reset();
  _SC_EXTERN bool IsValid() const;
  _SC_EXTERN bool Read(sc_char * buff, size_t buffLen, size_t & readBytes) const;
  _SC_EXTERN bool Write(sc_char * data, size_t dataLen, size_t & writtenBytes);
  _SC_EXTERN bool Seek(sc_stream_seek_origin origin, size_t offset);

  //! Check if current position at the end of file
  _SC_EXTERN bool Eof() const;

  //! Returns length of stream in bytes
  _SC_EXTERN size_t Size() const;

  //! Returns current position of stream
  _SC_EXTERN size_t Pos() const;

  //! Check if stream has a specified flag
  _SC_EXTERN bool HasFlag(sc_uint8 flag);

  template <typename Type>
  bool ReadType(Type & value)
  {
    size_t readBytes = 0;
    return Read((sc_char *)&value, sizeof(Type), readBytes) && (readBytes == sizeof(Type));
  }

protected:
  sc_stream * m_stream;
};

using ScStreamPtr = std::shared_ptr<ScStream>;

class ScStreamMemory : public ScStream
{
public:
  _SC_EXTERN explicit ScStreamMemory(MemoryBufferPtr const & buff);
  _SC_EXTERN virtual ~ScStreamMemory();

private:
  MemoryBufferPtr m_buffer;
};

class ScStreamConverter
{
public:
  static _SC_EXTERN bool StreamToString(ScStreamPtr const & stream, std::string & outString);
  static _SC_EXTERN ScStreamPtr StreamFromString(std::string const & str);
};

template <typename T>
inline ScStreamPtr ScStreamMakeReadT(T const & value)
{
  static_assert(std::is_arithmetic<T>::value, "Just simple arithmetic types are supported");
  return std::make_shared<ScStream>(
      (sc_char *)&value, sizeof(T), SC_STREAM_FLAG_READ | SC_STREAM_FLAG_SEEK | SC_STREAM_FLAG_TELL);
}

// TODO: implement with enable_if
inline ScStreamPtr ScStreamMakeRead(std::string const & value)
{
  return ScStreamConverter::StreamFromString(value);
}
inline ScStreamPtr ScStreamMakeRead(uint8_t const & value)
{
  return ScStreamMakeReadT(value);
}
inline ScStreamPtr ScStreamMakeRead(uint16_t const & value)
{
  return ScStreamMakeReadT(value);
}
inline ScStreamPtr ScStreamMakeRead(uint32_t const & value)
{
  return ScStreamMakeReadT(value);
}
inline ScStreamPtr ScStreamMakeRead(uint64_t const & value)
{
  return ScStreamMakeReadT(value);
}
inline ScStreamPtr ScStreamMakeRead(int8_t const & value)
{
  return ScStreamMakeReadT(value);
}
inline ScStreamPtr ScStreamMakeRead(int16_t const & value)
{
  return ScStreamMakeReadT(value);
}
inline ScStreamPtr ScStreamMakeRead(int32_t const & value)
{
  return ScStreamMakeReadT(value);
}
inline ScStreamPtr ScStreamMakeRead(int64_t const & value)
{
  return ScStreamMakeReadT(value);
}
inline ScStreamPtr ScStreamMakeRead(float const & value)
{
  return ScStreamMakeReadT(value);
}
inline ScStreamPtr ScStreamMakeRead(double const & value)
{
  return ScStreamMakeReadT(value);
}
