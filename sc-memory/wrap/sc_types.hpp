/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "sc_memory.h"

#include <string>
#include <map>
#include <assert.h>

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

	sc_type operator * () const
	{
		return mRealType;
	}

private:
	tRealType mRealType;
};
