/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_stream.hpp"


ScStream::ScStream()
    : mStream(0)
{
}

ScStream::ScStream(sc_stream * stream)
    : mStream(stream)
{
}

ScStream::ScStream(std::string const & fileName, sc_uint8 flags)
{
    mStream = sc_stream_file_new(fileName.c_str(), flags);
}

ScStream::ScStream(sc_char * buffer, sc_uint32 bufferSize, sc_uint8 flags)
{
    mStream = sc_stream_memory_new(buffer, bufferSize, flags, SC_FALSE);
}

ScStream::ScStream(sc_char const * buffer, sc_uint32 bufferSize, sc_uint8 flags)
{
    mStream = sc_stream_memory_new(buffer, bufferSize, flags, SC_FALSE);
}

ScStream::~ScStream()
{
    reset();
}

void ScStream::reset()
{
    if (mStream)
        sc_stream_free(mStream);
    mStream = 0;
}

bool ScStream::isValid() const
{
    return mStream != 0;
}

bool ScStream::read(sc_char * buff, sc_uint32 buffLen, sc_uint32 & readBytes) const
{
    check_expr(isValid());
    return sc_stream_read_data(mStream, buff, buffLen, &readBytes) == SC_RESULT_OK;
}

bool ScStream::write(sc_char * data, sc_uint32 dataLen, sc_uint32 & writtenBytes)
{
    check_expr(isValid());
    return sc_stream_write_data(mStream, data, dataLen, &writtenBytes) == SC_RESULT_OK;
}

bool ScStream::seek(sc_stream_seek_origin origin, sc_uint32 offset)
{
    check_expr(isValid());
    return sc_stream_seek(mStream, origin, offset) == SC_RESULT_OK;
}

bool ScStream::eof() const
{
    check_expr(isValid());
    return (sc_stream_eof(mStream) == SC_TRUE);
}

sc_uint32 ScStream::size() const
{
    check_expr(isValid());
    sc_uint32 len;
    if (sc_stream_get_length(mStream, &len) != SC_RESULT_OK)
        len = 0;

    return len;
}

sc_uint32 ScStream::pos() const
{
    check_expr(isValid());
    sc_uint32 pos;
    if (sc_stream_get_position(mStream, &pos) != SC_RESULT_OK)
        pos = 0;

    return pos;
}

bool ScStream::hasFlag(sc_uint8 flag)
{
    check_expr(isValid());
    return (sc_stream_check_flag(mStream, flag) == SC_TRUE);
}

void ScStream::init(sc_stream * stream)
{
    reset();
    mStream = stream;
}

// ---------------

ScStreamMemory::ScStreamMemory()
{
	reinit(MemoryBufferPtr());
}

ScStreamMemory::ScStreamMemory(MemoryBufferPtr const & buff)
{
	reinit(buff);
}

ScStreamMemory::~ScStreamMemory()
{
}

void ScStreamMemory::reinit(MemoryBufferPtr const & buff)
{
	mPos = 0;
	mBuffer = buff;
}

bool ScStreamMemory::isValid() const
{
	return mBuffer.isValid();
}

bool ScStreamMemory::read(sc_char * buff, sc_uint32 buffLen, sc_uint32 & readBytes) const
{
	assert(mBuffer.isValid());
	if (mPos < mBuffer->mSize)
	{
		readBytes = min(mBuffer->mSize - mPos, buffLen);
		memcpy(buff, (mBuffer->mData + mPos), readBytes);
		mPos += readBytes;
		return true;
	}

	readBytes = 0;
	return false;
}

bool ScStreamMemory::write(sc_char * data, sc_uint32 dataLen, sc_uint32 & writtenBytes)
{
	return false;
}

bool ScStreamMemory::seek(sc_stream_seek_origin origin, sc_uint32 offset)
{
	assert(mBuffer.isValid());
	switch (origin)
	{
	case SC_STREAM_SEEK_SET:
		if (offset > mBuffer->mSize)
			return false;
		mPos = offset;
		break;

	case SC_STREAM_SEEK_CUR:
		if (mPos + offset >= mBuffer->mSize)
			return false;
		mPos += offset;
		break;

	case SC_STREAM_SEEK_END:
		if (offset > mBuffer->mSize)
			return false;
		mPos = mBuffer->mSize - offset;
		break;
	};

	return true;
}

bool ScStreamMemory::eof() const
{
	assert(mBuffer.isValid());
	return mPos >= mBuffer->mSize;
}

sc_uint32 ScStreamMemory::size() const
{
	assert(mBuffer.isValid());
	return mBuffer->mSize;
}

sc_uint32 ScStreamMemory::pos() const
{
	assert(mBuffer.isValid());
	return mPos;
}

bool ScStreamMemory::hasFlag(sc_uint8 flag)
{
	return !(flag & SC_STREAM_FLAG_WRITE);
}

// --------------------------------
bool StreamConverter::streamToString(ScStream const & stream, std::string & outString)
{
	sc_uint32 const bytesCount = stream.size();
	if (bytesCount == 0)
		return false;

	char * data = new char[bytesCount];
	sc_uint32 readBytes;
	if (stream.read(data, bytesCount, readBytes) && (readBytes == bytesCount))
	{
		outString.assign(data, data + bytesCount);
	}
	delete []data;

	return true;

}

void StreamConverter::streamFromString(std::string const & str, ScStreamMemory & outStream)
{
	MemoryBufferPtr buff = MemoryBufferPtr(new MemoryBufferSafe(str.c_str(), (uint32_t)str.size()));
	outStream.reinit(buff);
}