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

	_SC_EXTERN virtual bool isValid() const = 0;

	_SC_EXTERN virtual bool read(sc_char * buff, sc_uint32 buffLen, sc_uint32 & readBytes) const = 0;

	_SC_EXTERN virtual bool write(sc_char * data, sc_uint32 dataLen, sc_uint32 & writtenBytes) = 0;

	_SC_EXTERN virtual bool seek(sc_stream_seek_origin origin, sc_uint32 offset) = 0;

	//! Check if current position at the end of file
	_SC_EXTERN virtual bool eof() const = 0;

	//! Returns lenght of stream in bytes
	_SC_EXTERN virtual sc_uint32 size() const = 0;

	//! Returns current position of stream
	_SC_EXTERN virtual sc_uint32 pos() const = 0;

	//! Check if stream has a specified flag
	_SC_EXTERN virtual bool hasFlag(sc_uint8 flag) = 0;

	template <typename Type>
	bool readType(Type & value)
	{
		sc_uint32 readBytes = 0;
		return read((sc_char*)&value, sizeof(Type), readBytes) && (readBytes == sizeof(Type));
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

	_SC_EXTERN void reset();

	_SC_EXTERN bool isValid() const;

	_SC_EXTERN bool read(sc_char * buff, sc_uint32 buffLen, sc_uint32 & readBytes) const;

	_SC_EXTERN bool write(sc_char * data, sc_uint32 dataLen, sc_uint32 & writtenBytes);

	_SC_EXTERN bool seek(sc_stream_seek_origin origin, sc_uint32 offset);

    //! Check if current position at the end of file
	_SC_EXTERN bool eof() const;

    //! Returns lenght of stream in bytes
	_SC_EXTERN sc_uint32 size() const;

    //! Returns current position of stream
	_SC_EXTERN sc_uint32 pos() const;

    //! Check if stream has a specified flag
	_SC_EXTERN bool hasFlag(sc_uint8 flag);

protected:
    //! Init with new stream object. Used by MemoryContext::getLinkContent
    void init(sc_stream * stream);

protected:
    sc_stream * mStream;
};

class ScStreamMemory : public IScStream
{
public:
	_SC_EXTERN explicit ScStreamMemory();
	_SC_EXTERN explicit ScStreamMemory(MemoryBufferPtr const & buff);
	_SC_EXTERN virtual ~ScStreamMemory();

	_SC_EXTERN void reinit(MemoryBufferPtr const & buff);

	_SC_EXTERN bool isValid() const;
	_SC_EXTERN bool read(sc_char * buff, sc_uint32 buffLen, sc_uint32 & readBytes) const;
	_SC_EXTERN bool write(sc_char * data, sc_uint32 dataLen, sc_uint32 & writtenBytes);
	_SC_EXTERN bool seek(sc_stream_seek_origin origin, sc_uint32 offset);
	_SC_EXTERN bool eof() const;
	_SC_EXTERN sc_uint32 size() const;
	_SC_EXTERN sc_uint32 pos() const;
	_SC_EXTERN bool hasFlag(sc_uint8 flag);

private:
	MemoryBufferPtr mBuffer;
	mutable sc_uint32 mPos;
};


SHARED_PTR_TYPE(IScStream)

class StreamConverter
{
public:

    static _SC_EXTERN bool streamToString(ScStream const & stream, std::string & outString);
	static _SC_EXTERN void streamFromString(std::string const & str, ScStreamMemory & outStream);
};

