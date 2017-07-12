/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sctpClient.hpp"
#include "sctpTypes.hpp"

namespace sctp
{

sc_uint8 mIterRange;
sc_uint32 mResultCount;
sc_uint32 mCurrentResult;
std::shared_ptr<char> mBuffer;

Iterator::Iterator(std::shared_ptr<char> buffer, sc_uint32 resultCount, sc_uint8 iterRange)
  : mIterRange(iterRange)
  , mResultCount(resultCount)
  , mCurrentResult(0)
  , mBuffer(buffer)
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

ScAddr Iterator::getValue(sc_uint8 idx) const
{
  SC_ASSERT(mCurrentResult > 0, ());
  sc_uint32 const offset = (mCurrentResult - 1) * sizeof(ScRealAddr) * mIterRange + sizeof(ScRealAddr) * idx;
  return ScAddr(*((ScRealAddr*)(mBuffer.get() + offset)));
}

// -----------------------------------------
_SC_EXTERN Client::Client(ISocket * socket)
  : m_cmdIdCounter(0)
  , m_socketImpl(socket)
{
}

_SC_EXTERN Client::~Client()
{
  if (m_socketImpl->IsConnected())
    m_socketImpl->Disconnect();
  delete m_socketImpl;
}

_SC_EXTERN bool Client::Connect(std::string const & address, std::string const & port)
{
  return m_socketImpl->Connect(address, port);
}

_SC_EXTERN void Client::Disconnect()
{
  m_socketImpl->Disconnect();
}

/// TODO: Implement error code return
_SC_EXTERN bool Client::IsElement(ScAddr const & addr)
{
  RequestElementCheck req;

  req.header.id = ++m_cmdIdCounter;
  req.header.flags = 0;
  req.header.commandType = SCTP_CMD_CHECK_ELEMENT;
  req.header.argsSize = SCTP_ADDR_SIZE;
  req.addr = *addr;

  if (m_socketImpl->WriteType(req) == sizeof(RequestElementCheck))
  {
    ResultHeader res;
    if (m_socketImpl->ReadType(res) == sizeof(ResultHeader))
      return (res.resultCode == SCTP_RESULT_OK);
  }


  return false;
}

_SC_EXTERN bool Client::EraseElement(ScAddr const & addr)
{
  RequestElementErase req;

  req.header.id = ++m_cmdIdCounter;
  req.header.flags = 0;
  req.header.commandType = SCTP_CMD_ERASE_ELEMENT;
  req.header.argsSize = SCTP_ADDR_SIZE;
  req.addr = *addr;

  if (m_socketImpl->WriteType(req) == sizeof(RequestElementErase))
  {
    ResultHeader res;
    if (m_socketImpl->ReadType(res) == sizeof(ResultHeader))
      return (res.resultCode == SCTP_RESULT_OK);
  }

  return false;
}

_SC_EXTERN ScAddr Client::CreateNode(sc_type type)
{
  RequestCreateNode req;
  req.header.id = ++m_cmdIdCounter;
  req.header.flags = 0;
  req.header.commandType = SCTP_CMD_CREATE_NODE;
  req.header.argsSize = sizeof(sc_type);
  req.type = type;

  if (m_socketImpl->WriteType(req) == sizeof(RequestCreateNode))
  {
    ResultHeader res;
    if ((m_socketImpl->ReadType(res) == sizeof(ResultHeader)) && (res.resultCode == SCTP_RESULT_OK))
    {
      ScRealAddr addr;
      SC_ASSERT(res.resultSize == sizeof(addr), ());
      if (m_socketImpl->ReadType(addr) == sizeof(addr))
        return ScAddr(addr);
    }
  }

  return ScAddr();
}

_SC_EXTERN ScAddr Client::CreateLink()
{
  RequestHeader req;
  req.id = ++m_cmdIdCounter;
  req.flags = 0;
  req.commandType = SCTP_CMD_CREATE_LINK;
  req.argsSize = 0;

  if (m_socketImpl->WriteType(req) == sizeof(RequestHeader))
  {
    ResultHeader res;
    if ((m_socketImpl->ReadType(res) == sizeof(ResultHeader)) && (res.resultCode == SCTP_RESULT_OK))
    {
      ScRealAddr addr;
      SC_ASSERT(res.resultSize == sizeof(addr), ());
      if (m_socketImpl->ReadType(addr) == sizeof(addr))
        return ScAddr(addr);
    }
  }

  return ScAddr();
}

_SC_EXTERN ScAddr Client::CreateArc(sc_type type, ScAddr const & addrBeg, ScAddr const & addrEnd)
{
  RequestCreateArc req;

  req.header.id = ++m_cmdIdCounter;
  req.header.flags = 0;
  req.header.commandType = SCTP_CMD_CREATE_ARC;
  req.header.argsSize = sizeof(sc_type) + SCTP_ADDR_SIZE * 2;
  req.type = type;
  req.addrBeg = *addrBeg;
  req.addrEnd = *addrEnd;

  if (m_socketImpl->WriteType(req) == sizeof(RequestCreateArc))
  {
    ResultHeader res;
    if ((m_socketImpl->ReadType(res) == sizeof(ResultHeader)) && (res.resultCode == SCTP_RESULT_OK))
    {
      ScRealAddr addr;
      SC_ASSERT(res.resultSize == sizeof(addr), ());
      if (m_socketImpl->ReadType(addr) == sizeof(addr))
        return ScAddr(addr);
    }
  }

  return ScAddr();
}

_SC_EXTERN sc_type Client::GetElementType(ScAddr const & addr)
{
  RequestElementType req;

  req.header.id = ++m_cmdIdCounter;
  req.header.flags = 0;
  req.header.commandType = SCTP_CMD_GET_ELEMENT_TYPE;
  req.header.argsSize = SCTP_ADDR_SIZE;
  req.addr = *addr;

  if (m_socketImpl->WriteType(req) == sizeof(RequestElementType))
  {
    ResultHeader resultHeader;
    if (ReadResultHeader(resultHeader) && (resultHeader.resultCode == SCTP_RESULT_OK))
    {
      if (resultHeader.resultSize == sizeof(sc_type))
      {
        sc_type resultType;
        if (m_socketImpl->ReadType(resultType) == sizeof(sc_type))
          return resultType;
      }
    }
  }

  return (sc_type)0;
}

_SC_EXTERN bool Client::SetElementSubtype(ScAddr const & addr, sc_type subtype)
{
  return false;
}

_SC_EXTERN bool Client::GetArcInfo(ScAddr const & arcAddr, ScAddr & outBegin, ScAddr & outEnd) const
{
  RequestArcInfo req;

  req.header.id = ++m_cmdIdCounter;
  req.header.flags = 0;
  req.header.commandType = SCTP_CMD_GET_ARC;
  req.header.argsSize = SCTP_ADDR_SIZE;
  req.addr = *arcAddr;

  if (m_socketImpl->WriteType(req) == sizeof(RequestArcInfo))
  {
    ResultHeader res;
    if ((m_socketImpl->ReadType(res) == sizeof(res)) && (res.resultCode == SCTP_RESULT_OK))
    {
      ScRealAddr addrs[2];
      SC_ASSERT(res.resultSize == sizeof(addrs), ());
      if (m_socketImpl->ReadType(addrs) == sizeof(addrs))
      {
        outBegin = ScAddr(addrs[0]);
        outEnd = ScAddr(addrs[1]);
        return true;
      }
    }
  }

  return false;
}

_SC_EXTERN bool Client::SetLinkContent(ScAddr const & addr, IScStreamPtr const & stream)
{
  RequestSetLinkContent req;

  req.header.id = ++m_cmdIdCounter;
  req.header.flags = 0;
  req.header.commandType = SCTP_CMD_SET_LINK_CONTENT;
  req.header.argsSize = sizeof(addr) + sizeof(sc_uint32) + static_cast<sc_uint32>(stream->Size());
  req.addr = *addr;
  req.size = static_cast<sc_uint32>(stream->Size());

  if (m_socketImpl->WriteType(req) == sizeof(RequestSetLinkContent))
  {
    char buff[1024];
    size_t readBytes;
    while (!stream->Eof())
    {
      if (!stream->Read(buff, 1024, readBytes))
        return false;
      if (m_socketImpl->Write(buff, readBytes) != static_cast<int>(readBytes))
        return false;
    }

    ResultHeader res;
    if (m_socketImpl->ReadType(res) == sizeof(res))
      return (res.resultCode == SCTP_RESULT_OK);
  }

  return false;
}

_SC_EXTERN bool Client::GetLinkContent(ScAddr const & addr, IScStreamPtr & stream)
{
  RequestGetLinkContent req;
  req.header.id = ++m_cmdIdCounter;
  req.header.flags = 0;
  req.header.commandType = SCTP_CMD_GET_LINK_CONTENT;
  req.header.argsSize = SCTP_ADDR_SIZE;
  req.addr = *addr;

  if (m_socketImpl->WriteType(req) == sizeof(RequestGetLinkContent))
  {
    ResultHeader res;
    if ((m_socketImpl->ReadType(res) == sizeof(res)) && (res.resultCode == SCTP_RESULT_OK))
    {
      char * buff = new char[res.resultSize];
      if (m_socketImpl->Read(buff, res.resultSize) == static_cast<int>(res.resultSize))
      {
        MemoryBufferPtr buffer(new MemoryBuffer(buff, res.resultSize));
        stream = IScStreamPtr(new ScStreamMemory(buffer));
        return true;
      }
    }
  }

  return false;
}

_SC_EXTERN bool Client::FindLinksByContent(IScStreamPtr const & stream, ScAddrList & found)
{
  return false;
}

bool Client::WriteSctpHeader(RequestHeader const & header)
{
  SC_ASSERT(m_socketImpl, ());

  return (m_socketImpl->WriteType(header) == sizeof(RequestHeader));
}

bool Client::ReadResultHeader(ResultHeader & outHeader)
{
  if (m_socketImpl->ReadType(outHeader) == sizeof(ResultHeader))
    return true;

  return false;
}


}
