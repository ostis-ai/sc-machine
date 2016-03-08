/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

extern "C"
{
#include "sc_memory.h"
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
