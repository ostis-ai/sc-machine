/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_stream.hpp"


ScStream::ScStream()
  : m_stream(0)
{
}

ScStream::ScStream(sc_stream * stream)
  : m_stream(stream)
{
}

ScStream::ScStream(std::string const & fileName, sc_uint8 flags)
{
  m_stream = sc_stream_file_new(fileName.c_str(), flags);
}

ScStream::ScStream(sc_char * buffer, size_t bufferSize, sc_uint8 flags)
{
  m_stream = sc_stream_memory_new(buffer, (sc_uint)bufferSize, flags, SC_FALSE);
}

ScStream::ScStream(sc_char const * buffer, size_t bufferSize, sc_uint8 flags)
{
  m_stream = sc_stream_memory_new(buffer, (sc_uint)bufferSize, flags, SC_FALSE);
}

ScStream::~ScStream()
{
  Reset();
}

void ScStream::Reset()
{
  if (m_stream)
    sc_stream_free(m_stream);
  m_stream = 0;
}

bool ScStream::IsValid() const
{
  return m_stream != 0;
}

bool ScStream::Read(sc_char * buff, size_t buffLen, size_t & readBytes) const
{
  SC_ASSERT(IsValid(), ());
  sc_uint32 readBytesNum = 0;
  bool const res = sc_stream_read_data(m_stream, buff, static_cast<sc_uint32>(buffLen), &readBytesNum) == SC_RESULT_OK;
  readBytes = (size_t)readBytesNum;
  return res;
}

bool ScStream::Write(sc_char * data, size_t dataLen, size_t & writtenBytes)
{
  SC_ASSERT(IsValid(), ());
  sc_uint32 writtenBytesNum = 0;
  bool res = sc_stream_write_data(m_stream, data, static_cast<sc_uint32>(dataLen), &writtenBytesNum) == SC_RESULT_OK;
  writtenBytes = (size_t)writtenBytesNum;
  return res;
}

bool ScStream::Seek(sc_stream_seek_origin origin, size_t offset)
{
  SC_ASSERT(IsValid(), ());
  return sc_stream_seek(m_stream, origin, static_cast<sc_uint32>(offset)) == SC_RESULT_OK;
}

bool ScStream::Eof() const
{
  SC_ASSERT(IsValid(), ());
  return (sc_stream_eof(m_stream) == SC_TRUE);
}

size_t ScStream::Size() const
{
  SC_ASSERT(IsValid(), ());
  sc_uint32 len;
  if (sc_stream_get_length(m_stream, &len) != SC_RESULT_OK)
    len = 0;

  return len;
}

size_t ScStream::Pos() const
{
  SC_ASSERT(IsValid(), ());
  sc_uint32 pos;
  if (sc_stream_get_position(m_stream, &pos) != SC_RESULT_OK)
    pos = 0;

  return pos;
}

bool ScStream::HasFlag(sc_uint8 flag)
{
  SC_ASSERT(IsValid(), ());
  return (sc_stream_check_flag(m_stream, flag) == SC_TRUE);
}

void ScStream::Init(sc_stream * stream)
{
  Reset();
  m_stream = stream;
}

// ---------------

ScStreamMemory::ScStreamMemory()
{
  Reinit(MemoryBufferPtr());
}

ScStreamMemory::ScStreamMemory(MemoryBufferPtr const & buff)
{
  Reinit(buff);
}

ScStreamMemory::~ScStreamMemory()
{
}

void ScStreamMemory::Reinit(MemoryBufferPtr const & buff)
{
  m_pos = 0;
  m_buffer = buff;
}

bool ScStreamMemory::IsValid() const
{
  return m_buffer.IsPtrValid();
}

bool ScStreamMemory::Read(sc_char * buff, size_t buffLen, size_t & readBytes) const
{
  SC_ASSERT(IsValid(), ());
  if (m_pos < m_buffer->Size())
  {
    readBytes = std::min(m_buffer->Size() - m_pos, buffLen);
    memcpy(buff, ((char*)m_buffer->CData() + m_pos), readBytes);
    m_pos += readBytes;
    return true;
  }

  readBytes = 0;
  return false;
}

bool ScStreamMemory::Write(sc_char * data, size_t dataLen, size_t & writtenBytes)
{
  return false;
}

bool ScStreamMemory::Seek(sc_stream_seek_origin origin, size_t offset)
{
  SC_ASSERT(m_buffer.IsPtrValid(), ());
  switch (origin)
  {
  case SC_STREAM_SEEK_SET:
    if (offset > m_buffer->Size())
      return false;
    m_pos = offset;
    break;

  case SC_STREAM_SEEK_CUR:
    if (m_pos + offset >= m_buffer->Size())
      return false;
    m_pos += offset;
    break;

  case SC_STREAM_SEEK_END:
    if (offset > m_buffer->Size())
      return false;
    m_pos = m_buffer->Size() - offset;
    break;
  };

  return true;
}

bool ScStreamMemory::Eof() const
{
  SC_ASSERT(m_buffer.IsPtrValid(), ());
  return (m_pos >= m_buffer->Size());
}

size_t ScStreamMemory::Size() const
{
  SC_ASSERT(m_buffer.IsPtrValid(), ());
  return m_buffer->Size();
}

size_t ScStreamMemory::Pos() const
{
  SC_ASSERT(m_buffer.IsPtrValid(), ());
  return m_pos;
}

bool ScStreamMemory::HasFlag(sc_uint8 flag)
{
  return !(flag & SC_STREAM_FLAG_WRITE);
}

// --------------------------------
bool ScStreamConverter::StreamToString(ScStream const & stream, std::string & outString)
{
  size_t const bytesCount = stream.Size();
  if (bytesCount == 0)
    return false;

  char * data = new char[bytesCount];
  size_t readBytes;
  if (stream.Read(data, bytesCount, readBytes) && (readBytes == bytesCount))
  {
    outString.assign(data, data + bytesCount);
  }
  delete []data;

  return true;

}

void ScStreamConverter::StreamFromString(std::string const & str, ScStreamMemory & outStream)
{
  MemoryBufferPtr buff = MemoryBufferPtr(new MemoryBufferSafe(str.c_str(), (uint32_t)str.size()));
  outStream.Reinit(buff);
}
