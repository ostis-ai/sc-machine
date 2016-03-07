/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_types.hpp"
#include "sc_addr.hpp"


struct ScTemplateItemValue
{
	ScTemplateItemValue()
	{
		mIsType = true;
	}

	ScTemplateItemValue(ScAddr const & addr)
	{
		setAddr(addr);
	}

	ScTemplateItemValue(ScType const & type)
	{
		setType(type);
	}

	void setAddr(ScAddr const & addr)
	{
		mIsType = false;
		mAddrValue = addr;
	}

	void setType(ScType const & type)
	{
		mIsType = true;
		mTypeValue = type;
	}

	bool mIsType;
	ScAddr mAddrValue;
	ScType mTypeValue;
};

class ScTemplateItem3
{
	friend class ScTemplate;
public:
	template <typename ParamType1, typename ParamType2, typename ParamType3>
	ScTemplateItem3(ParamType1 param1, ParamType2 param2, ParamType3 param3);



protected:
	ScTemplateItemValue mValues[3];
};


class ScTemplate
{

	template <typename ParamType1, typename ParamType2, typename ParamType3>
	ScTemplate & constr3(ParamType1 const & param1, ParamType2 const & param2, ParamType3 const & param3)
	{

	}

private:
	typedef std::map<std::string, ScAddr> tAddrsMap;
	tAddrsMap mReplacements;
};