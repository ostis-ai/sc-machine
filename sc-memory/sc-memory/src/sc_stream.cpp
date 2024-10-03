/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-memory/sc_stream.hpp"

namespace
{
#define CHECK_STREAM SC_ASSERT(IsValid(), "Used stream is invalid. Make sure that it's initialized")
}  // namespace

ScStream::ScStream()
  : m_stream(nullptr)
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

ScStream::ScStream(sc_char * buffer, size_t bufferSize, sc_uint8 flags, bool dataOwner)
{
  m_stream = sc_stream_memory_new(buffer, (sc_uint)bufferSize, flags, dataOwner);
}

ScStream::~ScStream()
{
  Reset();
}

void ScStream::Reset()
{
  if (m_stream)
    sc_stream_free(m_stream);
}

bool ScStream::IsValid() const
{
  return m_stream != nullptr;
}

bool ScStream::Read(sc_char * buff, size_t buffLen, size_t & readBytes) const
{
  CHECK_STREAM;

  sc_uint32 readBytesNum = 0;
  bool const res = sc_stream_read_data(m_stream, buff, static_cast<sc_uint32>(buffLen), &readBytesNum) == SC_RESULT_OK;
  readBytes = (size_t)readBytesNum;
  return res;
}

bool ScStream::Write(sc_char * data, size_t dataLen, size_t & writtenBytes)
{
  CHECK_STREAM;

  sc_uint32 writtenBytesNum = 0;
  bool res = sc_stream_write_data(m_stream, data, static_cast<sc_uint32>(dataLen), &writtenBytesNum) == SC_RESULT_OK;
  writtenBytes = (size_t)writtenBytesNum;
  return res;
}

bool ScStream::Seek(sc_stream_seek_origin origin, size_t offset)
{
  CHECK_STREAM;
  return sc_stream_seek(m_stream, origin, static_cast<sc_uint32>(offset)) == SC_RESULT_OK;
}

bool ScStream::Eof() const
{
  CHECK_STREAM;
  return (sc_stream_eof(m_stream) == SC_TRUE);
}

size_t ScStream::Size() const
{
  CHECK_STREAM;

  sc_uint32 len;
  if (sc_stream_get_length(m_stream, &len) != SC_RESULT_OK)
    len = 0;

  return len;
}

size_t ScStream::Pos() const
{
  CHECK_STREAM;

  sc_uint32 pos;
  if (sc_stream_get_position(m_stream, &pos) != SC_RESULT_OK)
    pos = 0;

  return pos;
}

bool ScStream::HasFlag(sc_uint8 flag)
{
  CHECK_STREAM;
  return (sc_stream_check_flag(m_stream, flag) == SC_TRUE);
}

// ---------------

ScStreamMemory::ScStreamMemory(MemoryBufferPtr const & buff)
  : ScStream(static_cast<sc_char const *>(buff->Data()), sc_uint(buff->Size()), SC_STREAM_FLAG_READ)
  , m_buffer(buff)
{
}

ScStreamMemory::~ScStreamMemory() = default;

// --------------------------------
bool ScStreamConverter::StreamToString(ScStreamPtr const & stream, std::string & outString)
{
  size_t const bytesCount = stream->Size();
  if (bytesCount == 0)
    return false;

  char * data = new char[bytesCount];
  size_t readBytes;
  if (stream->Read(data, bytesCount, readBytes) && (readBytes == bytesCount))
    outString.assign(data, data + bytesCount);

  delete[] data;

  return true;
}

ScStreamPtr ScStreamConverter::StreamFromString(std::string const & str)
{
  MemoryBufferPtr buff = MemoryBufferPtr(new MemoryBufferSafe(str.c_str(), (uint32_t)str.size()));
  return std::make_shared<ScStreamMemory>(buff);
}
