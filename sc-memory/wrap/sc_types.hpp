/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "sc_memory.h"

#include <string>
#include <map>

class ScMemoryCotext;
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


private:
	tRealType mRealType;
};
