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

#include <string>
#include <sstream>
#include <map>
#include <set>
#include <vector>
#include <list>
#include <array>
#include <unordered_set>
#include <unordered_map>
#include <assert.h>
#include <stdint.h>

#include <fstream>

#include "sc_defines.hpp"

typedef std::set<std::string> tStringSet;
typedef std::vector<std::string> tStringVector;
typedef std::map<std::string, std::string> tStringMap;

class ScMemoryContext;
class ScAddr;

class _SC_EXTERN ScType
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

	inline bool isEdge() const
	{
		return (mRealType & sc_type_arc_mask) != 0;
	}

	inline bool isNode() const
	{
		return (mRealType & sc_type_node) != 0;
	}

	inline bool isLink() const
	{
		return (mRealType & sc_type_link) != 0;
	}

	inline bool isConst() const
	{
		return (mRealType & sc_type_const) != 0;
	}

	inline bool isVar() const
	{
		return (mRealType & sc_type_var) != 0;
	}

	// Returns copy of this type, but with variable raplaced to const
	inline ScType asConst() const
	{
		return ScType((mRealType & ~sc_type_var) | sc_type_const);
	}

	// Returns copy of this type, but replace constancy type upward (metavar -> var -> const)
	inline ScType upConstType() const
	{
		/// TODO: metavar
		//if (isVar())
		return ScType((mRealType & ~sc_type_var) | sc_type_const); // copied from asConst for maximum perfomance
	}

	inline sc_type operator * () const
	{
		return mRealType;
	}

	ScType & operator() (tRealType bits)
	{
		mRealType |= bits;
		return *this;
	}

	bool operator == (ScType const & other)
	{
		return (mRealType == other.mRealType);
	}

	inline tRealType bitAnd(tRealType const & inMask) const
	{
		return (mRealType & inMask);
	}

	operator tRealType () const
	{
		return mRealType;
	}

private:
	tRealType mRealType;

public:
    static const ScType EDGE_UCOMMON;
    static const ScType EDGE_DCOMMON;

	static const ScType EDGE_UCOMMON_CONST;
	static const ScType EDGE_DCOMMON_CONST;

    static const ScType EDGE_ACCESS;

	static const ScType EDGE_ACCESS_CONST_POS_PERM;
	static const ScType EDGE_ACCESS_CONST_NEG_PERM;
	static const ScType EDGE_ACCESS_CONST_FUZ_PERM;
	static const ScType EDGE_ACCESS_CONST_POS_TEMP;
	static const ScType EDGE_ACCESS_CONST_NEG_TEMP;
	static const ScType EDGE_ACCESS_CONST_FUZ_TEMP;

	static const ScType EDGE_UCOMMON_VAR;
	static const ScType EDGE_DCOMMON_VAR;
	static const ScType EDGE_ACCESS_VAR_POS_PERM;
	static const ScType EDGE_ACCESS_VAR_NEG_PERM;
	static const ScType EDGE_ACCESS_VAR_FUZ_PERM;
	static const ScType EDGE_ACCESS_VAR_POS_TEMP;
	static const ScType EDGE_ACCESS_VAR_NEG_TEMP;
	static const ScType EDGE_ACCESS_VAR_FUZ_TEMP;
	


	static const ScType NODE;
	static const ScType LINK;

	static const ScType NODE_CONST;
	static const ScType NODE_VAR;
	
	static const ScType NODE_CONST_STRUCT;
	static const ScType NODE_CONST_TUPLE;
	static const ScType NODE_CONST_ROLE;
	static const ScType NODE_CONST_NOROLE;
	static const ScType NODE_CONST_CLASS;
	static const ScType NODE_CONST_ABSTRACT;
	static const ScType NODE_CONST_MATERIAL;

	static const ScType NODE_VAR_STRUCT;
	static const ScType NODE_VAR_TUPLE;
	static const ScType NODE_VAR_ROLE;
	static const ScType NODE_VAR_NOROLE;
	static const ScType NODE_VAR_CLASS;
	static const ScType NODE_VAR_ABSTRACT;
	static const ScType NODE_VAR_MATERIAL;
};
