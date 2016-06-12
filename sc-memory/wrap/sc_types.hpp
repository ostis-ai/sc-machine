/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "sc_memory.h"

#include <string>
#include <sstream>
#include <map>
#include <set>
#include <assert.h>
#include <stdint.h>

#include "sc_defines.hpp"

typedef std::set<std::string> tStringSet;

class ScMemoryContext;
class ScAddr;

class ScType
{
public:
	typedef sc_type tRealType;

	explicit ScType() : mRealType(0) 
	{
	}

	explicit ScType(tRealType type)
		: mRealType(type)
	{
	}

	bool isEdge() const
	{
		return (mRealType & sc_type_arc_mask) != 0;
	}

	bool isNode() const
	{
		return (mRealType & sc_type_node) != 0;
	}

	bool isLink() const
	{
		return (mRealType & sc_type_link) != 0;
	}

	bool isConst() const
	{
		return (mRealType & sc_type_const) != 0;
	}

	bool isVar() const
	{
		return (mRealType & sc_type_var) != 0;
	}

	// Returns copy of this type, but with variable raplaced to const
	ScType asConst() const
	{
		return ScType((mRealType & ~sc_type_var) | sc_type_const);
	}

	sc_type operator * () const
	{
		return mRealType;
	}

	bool operator == (ScType const & other)
	{
		return (mRealType == other.mRealType);
	}

	tRealType bitAnd(tRealType const & inMask) const
	{
		return (mRealType & inMask);
	}

private:
	tRealType mRealType;
};
