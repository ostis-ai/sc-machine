/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "wrap/sc_addr.hpp"
#include "wrap/sc_stream.hpp"
#include "wrap/sc_utils.hpp"

#include "sctpTypes.hpp"



namespace sctp
{

#define SCTP_ADDR_SIZE      (sizeof(sc::tRealAddr))

#pragma pack(push,1)
struct RequestHeader
{
    sc_uint8 commandType;
    sc_uint8 flags;
    sc_uint32 id;
    sc_uint32 argsSize;
};

struct BaseElementRequest
{
    RequestHeader header;
    sc::tRealAddr addr;
};

typedef BaseElementRequest RequestElementCheck;
typedef BaseElementRequest RequestElementErase;
typedef BaseElementRequest RequestElementType;
typedef BaseElementRequest RequestArcInfo;

struct RequestCreateNode
{
    RequestHeader header;
    sc_type type;
};

struct RequestCreateArc
{
    RequestHeader header;
    sc_type type;
    sc::tRealAddr addrBeg;
    sc::tRealAddr addrEnd;
};

typedef BaseElementRequest RequestGetLinkContent;

struct RequestSetLinkContent
{
	RequestHeader header;
	sc::tRealAddr addr;
	sc_uint32 size;

	// data appends later (because it has dynamic size)
};


/// ------------
struct ResultHeader
{
    sc_uint8 code;
    sc_uint32 id;
    sc_uint8 resultCode;
    sc_uint32 resultSize;
};


#pragma pack(pop)

class ISocket;

class Iterator
{
	friend class Client;

protected:
	_SC_EXTERN Iterator(sc::TSharedPointer<char> buffer, sc_uint32 resultCount, sc_uint8 iterRange);

public:
	_SC_EXTERN virtual ~Iterator();
	_SC_EXTERN bool next();
	_SC_EXTERN sc::Addr getValue(sc_uint8 idx) const;

private:
	sc_uint8 mIterRange;
	sc_uint32 mResultCount;
	sc_uint32 mCurrentResult;
	sc::TSharedPointer<char> mBuffer;
};

SHARED_PTR_TYPE(Iterator)

template <typename ParamType1, typename ParamType2, typename ParamType3>
sc_uint32 Iterator3ParamsT(char * buffer, ParamType1 const & param1, ParamType2 const & param2, ParamType3 const & param3);

template<> sc_uint32 Iterator3ParamsT<sc::Addr, sc_type, sc_type>(char * buffer, sc::Addr const & param1, sc_type const & param2, sc_type const & param3)
{
	buffer[0] = SCTP_ITERATOR_3F_A_A;
	sc::tRealAddr * addrBuff = (sc::tRealAddr*)(buffer + 1);
	*addrBuff = param1.getRealAddr();
	++addrBuff;
	sc_type * typeBuff = (sc_type*)addrBuff;
	*typeBuff = param2;
	++typeBuff;
	*typeBuff = param3;

	return 1 + sizeof(sc::tRealAddr) + sizeof(sc_type)* 2;
}

template<> sc_uint32 Iterator3ParamsT<sc::Addr, sc_type, sc::Addr>(char * buffer, sc::Addr const & param1, sc_type const & param2, sc::Addr const & param3)
{
	buffer[0] = SCTP_ITERATOR_3F_A_F;
	sc::tRealAddr * addrBuff = (sc::tRealAddr*)(buffer + 1);
	*addrBuff = param1.getRealAddr();
	++addrBuff;
	sc_type * typeBuff = (sc_type*)addrBuff;
	*typeBuff = param2;
	++typeBuff;
	addrBuff = (sc::tRealAddr*)typeBuff;
	*addrBuff = param3.getRealAddr();

	return 1 + sizeof(sc::tRealAddr) * 2 + sizeof(sc_type);
}

template<> sc_uint32 Iterator3ParamsT<sc_type, sc_type, sc::Addr>(char * buffer, sc_type const & param1, sc_type const & param2, sc::Addr const & param3)
{
	buffer[0] = SCTP_ITERATOR_3A_A_F;
	sc_type * typeBuff = (sc_type*)(buffer + 1);
	*typeBuff = param1;
	++typeBuff;
	*typeBuff = param2;
	++typeBuff;
	sc::tRealAddr * addrBuff = (sc::tRealAddr*)typeBuff;
	*addrBuff = param3.getRealAddr();
	
	return 1 + sizeof(sc::tRealAddr) + sizeof(sc_type) * 2;
}


template <typename ParamType1, typename ParamType2, typename ParamType3, typename ParamType4, typename ParamType5>
sc_uint32 Iterator5ParamsT(char * buffer, ParamType1 const & param1, ParamType2 const & param2, ParamType3 const & param3, ParamType4 const & param4, ParamType5 const & param5);


template <> sc_uint32 Iterator5ParamsT<sc::Addr, sc_type, sc_type, sc_type, sc::Addr>
	(char * buffer, sc::Addr const & param1, sc_type const & param2, sc_type const & param3, sc_type const & param4, sc::Addr const & param5)
{
	buffer[0] = SCTP_ITERATOR_5F_A_A_A_F;
	sc::tRealAddr * addrBuff = (sc::tRealAddr*)(buffer + 1);
	*addrBuff = param1.getRealAddr();
	++addrBuff;
	sc_type * typeBuff = (sc_type*)addrBuff;
	*typeBuff = param2;
	++typeBuff;
	*typeBuff = param3;
	++typeBuff;
	*typeBuff = param4;
	++typeBuff;
	addrBuff = (sc::tRealAddr*)typeBuff;
	*addrBuff = param5.getRealAddr();

	return 1 + sizeof(sc::tRealAddr) * 2 + sizeof(sc_type) * 3;
}

template <> sc_uint32 Iterator5ParamsT<sc_type, sc_type, sc::Addr, sc_type, sc::Addr>
	(char * buffer, sc_type const & param1, sc_type const & param2, sc::Addr const & param3, sc_type const & param4, sc::Addr const & param5)
{
	buffer[0] = SCTP_ITERATOR_5A_A_F_A_F;
	sc_type * typeBuff = (sc_type*)(buffer + 1);
	*typeBuff = param1;
	++typeBuff;
	*typeBuff = param2;
	++typeBuff;
	sc::tRealAddr * addrBuff = (sc::tRealAddr*)typeBuff;
	*addrBuff = param3.getRealAddr();
	++addrBuff;
	typeBuff = (sc_type*)addrBuff;
	*typeBuff = param4;
	++typeBuff;
	addrBuff = (sc::tRealAddr*)typeBuff;
	*addrBuff = param5.getRealAddr();

	return 1 + sizeof(sc::tRealAddr) * 2 + sizeof(sc_type) * 3;
}

template <> sc_uint32 Iterator5ParamsT<sc::Addr, sc_type, sc::Addr, sc_type, sc::Addr>
	(char * buffer, sc::Addr const & param1, sc_type const & param2, sc::Addr const & param3, sc_type const & param4, sc::Addr const & param5)
{
	buffer[0] = SCTP_ITERATOR_5F_A_F_A_F;
	sc::tRealAddr * addrBuff = (sc::tRealAddr*)(buffer + 1);
	*addrBuff = param1.getRealAddr();
	++addrBuff;
	sc_type * typeBuff = (sc_type*)addrBuff;
	*typeBuff = param2;
	++typeBuff;
	addrBuff = (sc::tRealAddr*)typeBuff;
	*addrBuff = param3.getRealAddr();
	typeBuff = (sc_type*)addrBuff;
	*typeBuff = param4;
	++typeBuff;
	addrBuff = (sc::tRealAddr*)typeBuff;
	*addrBuff = param5.getRealAddr();

	return 1 + sizeof(sc::tRealAddr) * 3 + sizeof(sc_type) * 2;
}

template <> sc_uint32 Iterator5ParamsT<sc_type, sc_type, sc::Addr, sc_type, sc_type>
	(char * buffer, sc_type const & param1, sc_type const & param2, sc::Addr const & param3, sc_type const & param4, sc_type const & param5)
{
	buffer[0] = SCTP_ITERATOR_5A_A_F_A_A;
	sc_type * typeBuff = (sc_type*)(buffer + 1);
	*typeBuff = param1;
	++typeBuff;
	*typeBuff = param2;
	++typeBuff;
	sc::tRealAddr * addrBuff = (sc::tRealAddr*)typeBuff;
	*addrBuff = param3.getRealAddr();
	++addrBuff;
	typeBuff = (sc_type*)addrBuff;
	*typeBuff = param4;
	++typeBuff;
	*typeBuff = param5;

	return 1 + sizeof(sc::tRealAddr) + sizeof(sc_type) * 4;
}

template <> sc_uint32 Iterator5ParamsT<sc::Addr, sc_type, sc_type, sc_type, sc_type>
	(char * buffer, sc::Addr const & param1, sc_type const & param2, sc_type const & param3, sc_type const & param4, sc_type const & param5)
{
	buffer[0] = SCTP_ITERATOR_5F_A_A_A_A;
	sc::tRealAddr * addrBuff = (sc::tRealAddr*)(buffer + 1);
	*addrBuff = param1.getRealAddr();
	++addrBuff;
	sc_type * typeBuff = (sc_type*)addrBuff;
	*typeBuff = param2;
	++typeBuff;
	*typeBuff = param3;
	++typeBuff;
	*typeBuff = param4;
	++typeBuff;
	*typeBuff = param5;

	return 1 + sizeof(sc::tRealAddr) + sizeof(sc_type) * 4;
}

template <> sc_uint32 Iterator5ParamsT<sc::Addr, sc_type, sc::Addr, sc_type, sc_type>
	(char * buffer, sc::Addr const & param1, sc_type const & param2, sc::Addr const & param3, sc_type const & param4, sc_type const & param5)
{
	buffer[0] = SCTP_ITERATOR_5F_A_F_A_A;
	sc::tRealAddr * addrBuff = (sc::tRealAddr*)(buffer + 1);
	*addrBuff = param1.getRealAddr();
	++addrBuff;
	sc_type * typeBuff = (sc_type*)addrBuff;
	*typeBuff = param2;
	++typeBuff;
	addrBuff = (sc::tRealAddr*)typeBuff;
	*addrBuff = param3.getRealAddr();
	++addrBuff;
	typeBuff = (sc_type*)addrBuff;
	*typeBuff = param4;
	++typeBuff;
	*typeBuff = param5;
	
	return 1 + sizeof(sc::tRealAddr) * 2 + sizeof(sc_type) * 3;
}

// ------------------------------------------------------
class Client
{
public:
    _SC_EXTERN explicit Client(ISocket * socket);
    _SC_EXTERN virtual ~Client();

    _SC_EXTERN bool connect(std::string const & address, std::string const & port);
    _SC_EXTERN void disconnect();

    //! Check if element exists with specified addr
    _SC_EXTERN bool isElement(sc::Addr const & addr);
    //! Erase element from sc-memory and returns true on success; otherwise returns false.
    _SC_EXTERN bool eraseElement(sc::Addr const & addr);

    _SC_EXTERN sc::Addr createNode(sc_type type);
    _SC_EXTERN sc::Addr createLink();
    _SC_EXTERN sc::Addr createArc(sc_type type, sc::Addr const & addrBeg, sc::Addr const & addrEnd);

    //! Returns type of sc-element. If there are any error, then returns 0
    _SC_EXTERN sc_type getElementType(sc::Addr const & addr);

    /*! Change subtype of sc-element (subtype & sc_type_element_mask == 0).
     * Return true, if there are no any errors; otherwise return false.
     */
    _SC_EXTERN bool setElementSubtype(sc::Addr const & addr, sc_type subtype);

    _SC_EXTERN bool getArcInfo(sc::Addr const & arcAddr, sc::Addr & outBegin, sc::Addr & outEnd) const;

	_SC_EXTERN bool setLinkContent(sc::Addr const & addr, sc::IStreamPtr const & stream);
	_SC_EXTERN bool getLinkContent(sc::Addr const & addr, sc::IStreamPtr & stream);

    //! Returns true, if any links found
	_SC_EXTERN bool findLinksByContent(sc::IStreamPtr const & stream, sc::tAddrList & found);

	template <typename ParamType1, typename ParamType2, typename ParamType3>
	_SC_EXTERN IteratorPtr iterator3(ParamType1 param1, ParamType2 param2, ParamType3 param3)
	{
		char buffer[128];
		sc_uint32 paramsSize = Iterator3ParamsT(buffer, param1, param2, param3);
		RequestHeader req;

		req.id = ++mCmdIdCounter;
		req.flags = 0;
		req.commandType = SCTP_CMD_ITERATE_ELEMENTS;
		req.argsSize = paramsSize;

		if (mSocketImpl->writeType(req) == sizeof(RequestHeader) && mSocketImpl->write(buffer, paramsSize) == paramsSize)
		{
			ResultHeader resultHeader;
			if (mSocketImpl->readType(resultHeader) == sizeof(ResultHeader))
			{
				sc_uint32 resultCount = 0;
				if (mSocketImpl->readType(resultCount) == sizeof(sc_uint32))
				{
					sc_uint32 const bufferSize = sizeof(sc::tRealAddr) * 3 * resultCount;
					if (bufferSize > 0)
					{
						sc::TSharedPointer<char> buffer = sc::TSharedPointer<char>(new char[bufferSize]);
						if (bufferSize > 0)
						{
							if (mSocketImpl->read(buffer._GetPtr(), bufferSize) == bufferSize)
								return IteratorPtr(new Iterator(buffer, resultCount, 3));
						}
					}
				}
			}
		}

		return IteratorPtr(nullptr);
	}

	/// TODO: possible merge 3 and 5 iterator in one template function
	template <typename ParamType1, typename ParamType2, typename ParamType3, typename ParamType4, typename ParamType5>
	_SC_EXTERN IteratorPtr iterator5(ParamType1 param1, ParamType2 param2, ParamType3 param3, ParamType4 param4, ParamType5 param5)
	{
		char buffer[128];
		sc_uint32 paramsSize = Iterator5ParamsT(buffer, param1, param2, param3, param4, param5);
		RequestHeader req;

		req.id = ++mCmdIdCounter;
		req.flags = 0;
		req.commandType = SCTP_CMD_ITERATE_ELEMENTS;
		req.argsSize = paramsSize;

		if (mSocketImpl->writeType(req) == sizeof(RequestHeader) && mSocketImpl->write(buffer, paramsSize) == paramsSize)
		{
			ResultHeader resultHeader;
			if (mSocketImpl->readType(resultHeader) == sizeof(ResultHeader))
			{
				sc_uint32 resultCount = 0;
				if (mSocketImpl->readType(resultCount) == sizeof(sc_uint32))
				{
					sc_uint32 const bufferSize = sizeof(sc::tRealAddr) * 5 * resultCount;
					if (bufferSize > 0)
					{
						sc::TSharedPointer<char> buffer = sc::TSharedPointer<char>(new char[bufferSize]);
						if (bufferSize > 0)
						{
							if (mSocketImpl->read(buffer._GetPtr(), bufferSize) == bufferSize)
								return IteratorPtr(new Iterator(buffer, resultCount, 5));
						}
					}
				}
			}
		}

		return IteratorPtr(nullptr);
	}


private:
    bool writeSctpHeader(RequestHeader const & header);
    /// Buffer must have a correct size
    bool readResultHeader(ResultHeader & outHeader);

    ///! TODO : Iterators
private:

    mutable sc_uint32 mCmdIdCounter;
    ISocket * mSocketImpl;
};

}
