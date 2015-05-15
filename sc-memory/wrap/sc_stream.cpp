/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_stream.hpp"

namespace sc
{

Stream::Stream()
    : mStream(0)
{
}

Stream::Stream(sc_stream * stream)
    : mStream(stream)
{
}

Stream::Stream(std::string const & fileName, sc_uint8 flags)
{
    mStream = sc_stream_file_new(fileName.c_str(), flags);
}

Stream::Stream(sc_char * buffer, sc_uint32 bufferSize, sc_uint8 flags)
{
    mStream = sc_stream_memory_new(buffer, bufferSize, flags, SC_FALSE);
}

Stream::Stream(sc_char const * buffer, sc_uint32 bufferSize, sc_uint8 flags)
{
    mStream = sc_stream_memory_new(buffer, bufferSize, flags, SC_FALSE);
}

Stream::~Stream()
{
    reset();
}

void Stream::reset()
{
    if (mStream)
        sc_stream_free(mStream);
    mStream = 0;
}

bool Stream::isValid() const
{
    return mStream != 0;
}

bool Stream::read(sc_char * buff, sc_uint32 buffLen, sc_uint32 & readBytes)
{
    check(isValid());
    return sc_stream_read_data(mStream, buff, buffLen, &readBytes) == SC_RESULT_OK;
}

bool Stream::write(sc_char * data, sc_uint32 dataLen, sc_uint32 & writtenBytes)
{
    check(isValid());
    return sc_stream_write_data(mStream, data, dataLen, &writtenBytes) == SC_RESULT_OK;
}

bool Stream::seek(sc_stream_seek_origin origin, sc_uint32 offset)
{
    check(isValid());
    return sc_stream_seek(mStream, origin, offset) == SC_RESULT_OK;
}

bool Stream::eof() const
{
    check(isValid());
    return sc_stream_eof(mStream) == SC_TRUE;
}

sc_uint32 Stream::size() const
{
    check(isValid());
    sc_uint32 len;
    if (sc_stream_get_length(mStream, &len) != SC_RESULT_OK)
        len = 0;

    return len;
}

sc_uint32 Stream::pos() const
{
    check(isValid());
    sc_uint32 pos;
    if (sc_stream_get_position(mStream, &pos) != SC_RESULT_OK)
        pos = 0;

    return pos;
}

bool Stream::hasFlag(sc_uint8 flag)
{
    check(isValid());
    return sc_stream_check_flag(mStream, flag) == SC_TRUE;
}

void Stream::init(sc_stream * stream)
{
    reset();
    mStream = stream;
}

}
