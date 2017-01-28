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

  _SC_EXTERN virtual bool Read(sc_char * buff, sc_uint32 buffLen, sc_uint32 & readBytes) const = 0;

  _SC_EXTERN virtual bool Write(sc_char * data, sc_uint32 dataLen, sc_uint32 & writtenBytes) = 0;

  _SC_EXTERN virtual bool Seek(sc_stream_seek_origin origin, sc_uint32 offset) = 0;

  //! Check if current position at the end of file
  _SC_EXTERN virtual bool Eof() const = 0;

  //! Returns lenght of stream in bytes
  _SC_EXTERN virtual sc_uint32 Size() const = 0;

  //! Returns current position of stream
  _SC_EXTERN virtual sc_uint32 Pos() const = 0;

  //! Check if stream has a specified flag
  _SC_EXTERN virtual bool HasFlag(sc_uint8 flag) = 0;

  template <typename Type>
  bool ReadType(Type & value)
  {
    sc_uint32 readBytes = 0;
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
  _SC_EXTERN explicit ScStream(sc_char * buffer, sc_uint32 bufferSize, sc_uint8 flags);
  _SC_EXTERN explicit ScStream(sc_char const * buffer, sc_uint32 bufferSize, sc_uint8 flags);

  _SC_EXTERN ~ScStream();

  _SC_EXTERN void Reset();

  _SC_EXTERN bool IsValid() const;

  _SC_EXTERN bool Read(sc_char * buff, sc_uint32 buffLen, sc_uint32 & readBytes) const;

  _SC_EXTERN bool Write(sc_char * data, sc_uint32 dataLen, sc_uint32 & writtenBytes);

  _SC_EXTERN bool Seek(sc_stream_seek_origin origin, sc_uint32 offset);

  //! Check if current position at the end of file
  _SC_EXTERN bool Eof() const;

  //! Returns lenght of stream in bytes
  _SC_EXTERN sc_uint32 Size() const;

  //! Returns current position of stream
  _SC_EXTERN sc_uint32 Pos() const;

  //! Check if stream has a specified flag
  _SC_EXTERN bool HasFlag(sc_uint8 flag);

protected:
  //! Init with new stream object. Used by MemoryContext::getLinkContent
  void Init(sc_stream * stream);

protected:
  sc_stream * m_stream;
};

class ScStreamMemory : public IScStream
{
public:
  _SC_EXTERN explicit ScStreamMemory();
  _SC_EXTERN explicit ScStreamMemory(MemoryBufferPtr const & buff);
  _SC_EXTERN virtual ~ScStreamMemory();

  _SC_EXTERN void Reinit(MemoryBufferPtr const & buff);

  _SC_EXTERN bool IsValid() const;
  _SC_EXTERN bool Read(sc_char * buff, sc_uint32 buffLen, sc_uint32 & readBytes) const;
  _SC_EXTERN bool Write(sc_char * data, sc_uint32 dataLen, sc_uint32 & writtenBytes);
  _SC_EXTERN bool Seek(sc_stream_seek_origin origin, sc_uint32 offset);
  _SC_EXTERN bool Eof() const;
  _SC_EXTERN sc_uint32 Size() const;
  _SC_EXTERN sc_uint32 Pos() const;
  _SC_EXTERN bool HasFlag(sc_uint8 flag);

private:
  MemoryBufferPtr m_buffer;
  mutable sc_uint32 m_pos;
};


SHARED_PTR_TYPE(IScStream)

class ScStreamConverter
{
  public:

  static _SC_EXTERN bool StreamToString(ScStream const & stream, std::string & outString);
  static _SC_EXTERN void StreamFromString(std::string const & str, ScStreamMemory & outStream);
};

