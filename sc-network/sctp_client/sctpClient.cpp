/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sctpClient.hpp"
#include "sctpTypes.hpp"

namespace sctp
{


Iterator::Iterator(sc::TSharedPointer<char> buffer, sc_uint32 resultCount, sc_uint8 iterRange)
	: mBuffer(buffer)
	, mResultCount(resultCount)
	, mCurrentResult(0)
	, mIterRange(iterRange)
{
}

Iterator::~Iterator()
{
}

bool Iterator::next()
{
	++mCurrentResult;
	return (mCurrentResult <= mResultCount);
}

sc::Addr Iterator::getValue(sc_uint8 idx) const
{
	check_expr(mCurrentResult > 0);
	sc_uint32 const offset = (mCurrentResult - 1) * sizeof(sc::tRealAddr) * mIterRange + sizeof(sc::tRealAddr) * idx;
	return sc::Addr(*((sc::tRealAddr*)(mBuffer._GetPtr() + offset)));
}

// -----------------------------------------
_SC_EXTERN Client::Client(ISocket * socket)
    : mCmdIdCounter(0)
    , mSocketImpl(socket)
{
}

_SC_EXTERN Client::~Client()
{
    if (mSocketImpl->isConnected())
        mSocketImpl->disconnect();
    delete mSocketImpl;
}

_SC_EXTERN bool Client::connect(std::string const & address, std::string const & port)
{
    return mSocketImpl->connect(address, port);
}

_SC_EXTERN void Client::disconnect()
{
    mSocketImpl->disconnect();
}

/// TODO: Implement error code return
_SC_EXTERN bool Client::isElement(sc::Addr const & addr)
{
    RequestElementCheck req;

    req.header.id = ++mCmdIdCounter;
    req.header.flags = 0;
    req.header.commandType = SCTP_CMD_CHECK_ELEMENT;
    req.header.argsSize = SCTP_ADDR_SIZE;
    req.addr = addr.getRealAddr();

    if (mSocketImpl->writeType(req) == sizeof(RequestElementCheck))
    {
        ResultHeader res;
        if (mSocketImpl->readType(res) == sizeof(ResultHeader))
            return (res.resultCode == SCTP_RESULT_OK);
    }


    return false;
}

_SC_EXTERN bool Client::eraseElement(sc::Addr const & addr)
{
    RequestElementErase req;

    req.header.id = ++mCmdIdCounter;
    req.header.flags = 0;
    req.header.commandType = SCTP_CMD_ERASE_ELEMENT;
    req.header.argsSize = SCTP_ADDR_SIZE;
    req.addr = addr.getRealAddr();

    if (mSocketImpl->writeType(req) == sizeof(RequestElementErase))
    {
        ResultHeader res;
        if (mSocketImpl->readType(res) == sizeof(ResultHeader))
            return (res.resultCode == SCTP_RESULT_OK);
    }

    return false;
}

_SC_EXTERN sc::Addr Client::createNode(sc_type type)
{
    RequestCreateNode req;
    req.header.id = ++mCmdIdCounter;
    req.header.flags = 0;
    req.header.commandType = SCTP_CMD_CREATE_NODE;
    req.header.argsSize = sizeof(sc_type);
    req.type = type;

    if (mSocketImpl->writeType(req) == sizeof(RequestCreateNode))
    {
        ResultHeader res;
        if ((mSocketImpl->readType(res) == sizeof(ResultHeader)) && (res.resultCode == SCTP_RESULT_OK))
        {
            sc::tRealAddr addr;
            assert(res.resultSize == sizeof(addr));
            if (mSocketImpl->readType(addr) == sizeof(addr))
                return sc::Addr(addr);
        }
    }

    return sc::Addr();
}

_SC_EXTERN sc::Addr Client::createLink()
{
	RequestHeader req;
	req.id = ++mCmdIdCounter;
	req.flags = 0;
	req.commandType = SCTP_CMD_CREATE_LINK;
	req.argsSize = 0;

	if (mSocketImpl->writeType(req) == sizeof(RequestHeader))
	{
		ResultHeader res;
		if ((mSocketImpl->readType(res) == sizeof(ResultHeader)) && (res.resultCode == SCTP_RESULT_OK))
		{
			sc::tRealAddr addr;
			assert(res.resultSize == sizeof(addr));
			if (mSocketImpl->readType(addr) == sizeof(addr))
				return sc::Addr(addr);
		}
	}

    return sc::Addr();
}

_SC_EXTERN sc::Addr Client::createArc(sc_type type, sc::Addr const & addrBeg, sc::Addr const & addrEnd)
{
    RequestCreateArc req;

    req.header.id = ++mCmdIdCounter;
    req.header.flags = 0;
    req.header.commandType = SCTP_CMD_CREATE_ARC;
    req.header.argsSize = sizeof(sc_type) + SCTP_ADDR_SIZE * 2;
    req.type = type;
    req.addrBeg = addrBeg.getRealAddr();
    req.addrEnd = addrEnd.getRealAddr();

    if (mSocketImpl->writeType(req) == sizeof(RequestCreateArc))
    {
        ResultHeader res;
        if ((mSocketImpl->readType(res) == sizeof(ResultHeader)) && (res.resultCode == SCTP_RESULT_OK))
        {
            sc::tRealAddr addr;
            assert(res.resultSize == sizeof(addr));
            if (mSocketImpl->readType(addr) == sizeof(addr))
                return sc::Addr(addr);
        }
    }

    return sc::Addr();
}

_SC_EXTERN sc_type Client::getElementType(sc::Addr const & addr)
{
    RequestElementType req;

    req.header.id = ++mCmdIdCounter;
    req.header.flags = 0;
    req.header.commandType = SCTP_CMD_GET_ELEMENT_TYPE;
    req.header.argsSize = SCTP_ADDR_SIZE;
    req.addr = addr.getRealAddr();

    if (mSocketImpl->writeType(req) == sizeof(RequestElementType))
    {
        ResultHeader resultHeader;
        if (readResultHeader(resultHeader) && (resultHeader.resultCode == SCTP_RESULT_OK))
        {
            if (resultHeader.resultSize == sizeof(sc_type))
            {
                sc_type resultType;
                if (mSocketImpl->readType(resultType) == sizeof(sc_type))
                    return resultType;
            }
        }
    }

    return (sc_type)0;
}

_SC_EXTERN bool Client::setElementSubtype(sc::Addr const & addr, sc_type subtype)
{
    return false;
}

_SC_EXTERN bool Client::getArcInfo(sc::Addr const & arcAddr, sc::Addr & outBegin, sc::Addr & outEnd) const
{
    RequestArcInfo req;

    req.header.id = ++mCmdIdCounter;
    req.header.flags = 0;
    req.header.commandType = SCTP_CMD_GET_ARC;
    req.header.argsSize = SCTP_ADDR_SIZE;
    req.addr = arcAddr.getRealAddr();

    if (mSocketImpl->writeType(req) == sizeof(RequestArcInfo))
    {
        ResultHeader res;
        if ((mSocketImpl->readType(res) == sizeof(res)) && (res.resultCode == SCTP_RESULT_OK))
        {
            sc::tRealAddr addrs[2];
            assert(res.resultSize == sizeof(addrs));
            if (mSocketImpl->readType(addrs) == sizeof(addrs))
            {
                outBegin = sc::Addr(addrs[0]);
                outEnd = sc::Addr(addrs[1]);
                return true;
            }
        }
    }

    return false;
}

_SC_EXTERN bool Client::setLinkContent(sc::Addr const & addr, sc::IStreamPtr const & stream)
{
	RequestSetLinkContent req;
	
	req.header.id = ++mCmdIdCounter;
	req.header.flags = 0;
	req.header.commandType = SCTP_CMD_SET_LINK_CONTENT;
	req.header.argsSize = sizeof(addr) + sizeof(sc_uint32) + stream->size();
	req.addr = addr.getRealAddr();
	req.size = stream->size();

	if (mSocketImpl->writeType(req) == sizeof(RequestSetLinkContent))
	{
		char buff[1024];
		sc_uint32 readBytes;
		while (!stream->eof())
		{
			if (!stream->read(buff, 1024, readBytes))
				return false;
			if (mSocketImpl->write(buff, readBytes) != readBytes)
				return false;
		}

		ResultHeader res;
		if (mSocketImpl->readType(res) == sizeof(res))
			return (res.resultCode == SCTP_RESULT_OK);
	}

    return false;
}

_SC_EXTERN bool Client::getLinkContent(sc::Addr const & addr, sc::IStreamPtr & stream)
{
	RequestGetLinkContent req;
	req.header.id = ++mCmdIdCounter;
	req.header.flags = 0;
	req.header.commandType = SCTP_CMD_GET_LINK_CONTENT;
	req.header.argsSize = SCTP_ADDR_SIZE;
	req.addr = addr.getRealAddr();

	if (mSocketImpl->writeType(req) == sizeof(RequestGetLinkContent))
	{
		ResultHeader res;
		if ((mSocketImpl->readType(res) == sizeof(res)) && (res.resultCode == SCTP_RESULT_OK))
		{
			char * buff = new char[res.resultSize];
			if (mSocketImpl->read(buff, res.resultSize) == res.resultSize)
			{
				sc::MemoryBufferPtr buffer(new sc::MemoryBuffer(buff, res.resultSize));
				stream = sc::IStreamPtr(new sc::StreamMemory(buffer));
				return true;
			}
		}
	}

    return false;
}

_SC_EXTERN bool Client::findLinksByContent(sc::IStreamPtr const & stream, sc::tAddrList & found)
{
    return false;
}

bool Client::writeSctpHeader(RequestHeader const & header)
{
    assert(mSocketImpl);

    return (mSocketImpl->writeType(header) == sizeof(RequestHeader));
}

bool Client::readResultHeader(ResultHeader & outHeader)
{
    if (mSocketImpl->readType(outHeader) == sizeof(ResultHeader))
        return true;

    return false;
}


}
