/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

extern "C"
{
#include "sc_memory_headers.h"
}

#include "sc_types.hpp"
#include "sc_utils.hpp"

namespace sc
{

class _SC_WRAP_EXTERN Stream
{
    friend class MemoryContext;

public:
    explicit Stream();
    explicit Stream(sc_stream * stream);
    explicit Stream(std::string const & fileName, sc_uint8 flags);
    explicit Stream(sc_char * buffer, sc_uint32 bufferSize, sc_uint8 flags);
    explicit Stream(sc_char const * buffer, sc_uint32 bufferSize, sc_uint8 flags);

    ~Stream();

    void reset();

    bool isValid() const;

    bool read(sc_char * buff, sc_uint32 buffLen, sc_uint32 & readBytes);

    bool write(sc_char * data, sc_uint32 dataLen, sc_uint32 & writtenBytes);

    bool seek(sc_stream_seek_origin origin, sc_uint32 offset);

    //! Check if current position at the end of file
    bool eof() const;

    //! Returns lenght of stream in bytes
    sc_uint32 size() const;

    //! Returns current position of stream
    sc_uint32 pos() const;

    //! Check if stream has a specified flag
    bool hasFlag(sc_uint8 flag);

protected:
    //! Init with new stream object. Used by MemoryContext::getLinkContent
    void init(sc_stream * stream);

protected:
    sc_stream * mStream;
};


}
