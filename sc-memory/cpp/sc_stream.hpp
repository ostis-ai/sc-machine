/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

extern "C"
{
#include "sc-memory/sc_memory_headers.h"
}

#include "sc_types.hpp"
#include "sc_utils.hpp"


class IScStream
{
public:
  _SC_EXTERN virtual ~IScStream() {};

  _SC_EXTERN virtual bool IsValid() const = 0;

  _SC_EXTERN virtual bool Read(sc_char * buff, size_t buffLen, size_t & readBytes) const = 0;

  _SC_EXTERN virtual bool Write(sc_char * data, size_t dataLen, size_t & writtenBytes) = 0;

  _SC_EXTERN virtual bool Seek(sc_stream_seek_origin origin, size_t offset) = 0;

  //! Check if current position at the end of file
  _SC_EXTERN virtual bool Eof() const = 0;

  //! Returns lenght of stream in bytes
  _SC_EXTERN virtual size_t Size() const = 0;

  //! Returns current position of stream
  _SC_EXTERN virtual size_t Pos() const = 0;

  //! Check if stream has a specified flag
  _SC_EXTERN virtual bool HasFlag(sc_uint8 flag) = 0;

  template <typename Type>
  bool ReadType(Type & value)
  {
    size_t readBytes = 0;
    return Read((sc_char*)&value, sizeof(Type), readBytes) && (readBytes == sizeof(Type));
  }
};


class ScStream : public IScStream
{
  friend class ScMemoryContext;

public:
  _SC_EXTERN explicit ScStream();
  _SC_EXTERN explicit ScStream(sc_stream * stream);
  _SC_EXTERN explicit ScStream(std::string const & fileName, sc_uint8 flags);
  _SC_EXTERN explicit ScStream(sc_char * buffer, size_t bufferSize, sc_uint8 flags);
  _SC_EXTERN explicit ScStream(sc_char const * buffer, size_t bufferSize, sc_uint8 flags);

  _SC_EXTERN ~ScStream();

  _SC_EXTERN void Reset();
  _SC_EXTERN bool IsValid() const override;
  _SC_EXTERN bool Read(sc_char * buff, size_t buffLen, size_t & readBytes) const override;
  _SC_EXTERN bool Write(sc_char * data, size_t dataLen, size_t & writtenBytes) override;
  _SC_EXTERN bool Seek(sc_stream_seek_origin origin, size_t offset) override;

  //! Check if current position at the end of file
  _SC_EXTERN bool Eof() const override;

  //! Returns lenght of stream in bytes
  _SC_EXTERN size_t Size() const override;

  //! Returns current position of stream
  _SC_EXTERN size_t Pos() const override;

  //! Check if stream has a specified flag
  _SC_EXTERN bool HasFlag(sc_uint8 flag) override;

protected:
  //! Init with new stream object. Used by MemoryContext::getLinkContent
  void Init(sc_stream * stream);

protected:
  sc_stream * m_stream;
};

using ScStreamPtr = TSharedPointer<ScStream>;

class ScStreamMemory : public IScStream
{
public:
  _SC_EXTERN explicit ScStreamMemory();
  _SC_EXTERN explicit ScStreamMemory(MemoryBufferPtr const & buff);
  _SC_EXTERN virtual ~ScStreamMemory();

  _SC_EXTERN void Reinit(MemoryBufferPtr const & buff);

  _SC_EXTERN bool IsValid() const override;
  _SC_EXTERN bool Read(sc_char * buff, size_t buffLen, size_t & readBytes) const override;
  _SC_EXTERN bool Write(sc_char * data, size_t dataLen, size_t & writtenBytes) override;
  _SC_EXTERN bool Seek(sc_stream_seek_origin origin, size_t offset) override;
  _SC_EXTERN bool Eof() const override;
  _SC_EXTERN size_t Size() const override;
  _SC_EXTERN size_t Pos() const override;
  _SC_EXTERN bool HasFlag(sc_uint8 flag) override;

private:
  MemoryBufferPtr m_buffer;
  mutable size_t m_pos;
};


SHARED_PTR_TYPE(IScStream)

class ScStreamConverter
{
  public:

  static _SC_EXTERN bool StreamToString(ScStream const & stream, std::string & outString);
  static _SC_EXTERN void StreamFromString(std::string const & str, ScStreamMemory & outStream);
};

template <typename T>
ScStreamPtr MakeReadStreamT(T const & value)
{
  return ScStreamPtr(new ScStream((sc_char*)&value, sizeof(T), SC_STREAM_FLAG_READ | SC_STREAM_FLAG_SEEK | SC_STREAM_FLAG_TELL));
}
