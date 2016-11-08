/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

extern "C"
{
#include "sc-memory/sc_memory.h"
}

#include <vector>
#include <list>

#include "sc_types.hpp"

typedef sc_addr tRealAddr;


class _SC_EXTERN ScAddr
{
	friend class ScMemoryContext;

    template <typename ParamType1, typename ParamType2, typename ParamType3> friend class TIterator3;
    template <typename ParamType1, typename ParamType2, typename ParamType3, typename ParamType4, typename ParamType5> friend class TIterator5;

public:
    bool isValid() const;
    void reset();

	bool operator == (ScAddr const & other) const;
	bool operator != (ScAddr const & other) const;
	tRealAddr operator * () const;

	/// TODO: remove and replace by operator * ()
    tRealAddr const & getRealAddr() const;

	explicit ScAddr();
	explicit ScAddr(sc_addr const & addr);

protected:
    tRealAddr mRealAddr;
};

typedef std::vector<ScAddr> tAddrVector;
typedef std::list<ScAddr> tAddrList;


struct RealAddrLessFunc
{
	bool operator () (tRealAddr const & a, tRealAddr const & b) const
	{
		if (a.seg < b.seg)
			return true;

		if (a.seg > b.seg)
			return false;

		return (a.offset < b.offset);
	}
};

struct ScAddLessFunc
{
	bool operator () (ScAddr const & a, ScAddr const & b)
	{
		return RealAddrLessFunc()(a.getRealAddr(), b.getRealAddr());
	}
};

// hash functions
template <typename HashType>
struct ScAddrHashFunc
{
	HashType operator () (ScAddr const & addr);
};

template <> struct ScAddrHashFunc < uint32_t >
{
    uint32_t operator() (ScAddr const & addr) const
	{
		return SC_ADDR_LOCAL_TO_INT(addr.getRealAddr());
	}
};
