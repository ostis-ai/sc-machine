/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_template.hpp"
#include "sc_memory.hpp"

#include <algorithm>

ScTemplate::ScTemplate(size_t BufferedNum)
	: mIsCacheValid(false)
{
	mConstructions.reserve(BufferedNum);
	mCurrentReplPos = 0;
}

ScTemplate & ScTemplate::operator() (ScTemplateItemValue const & param1, ScTemplateItemValue const & param2, ScTemplateItemValue const & param3)
{
	return triple(param1, param2, param3);
}

ScTemplate & ScTemplate::operator() (ScTemplateItemValue const & param1, ScTemplateItemValue const & param2, ScTemplateItemValue const & param3,
	ScTemplateItemValue const & param4, ScTemplateItemValue const & param5)
{
	return tripleWithRelation(param1, param2, param3, param4, param5);
}

void ScTemplate::clear()
{
	mConstructions.clear();
	mReplacements.clear();
	mCurrentReplPos = 0;

	mIsCacheValid = false;
}

bool ScTemplate::isSearchCacheValid() const
{
	return (mIsCacheValid && (mSearchCachedOrder.size() == mConstructions.size()));
}

bool ScTemplate::isGenerateCacheValid() const
{
	return (mIsCacheValid && (mGenerateCachedOrder.size() == mConstructions.size()));
}

bool ScTemplate::hasReplacement(std::string const & repl) const
{
	return (mReplacements.find(repl) != mReplacements.end());
}

ScTemplate & ScTemplate::triple(ScTemplateItemValue const & param1, ScTemplateItemValue const & param2, ScTemplateItemValue const & param3)
{
	size_t const replPos = mConstructions.size() * 3;
	mConstructions.emplace_back(ScTemplateConstr3(param1, param2, param3, mConstructions.size()));

	ScTemplateConstr3 & constr3 = mConstructions.back();
	for (size_t i = 0; i < 3; ++i)
	{
		ScTemplateItemValue & value = constr3.mValues[i];
		if (value.mItemType == ScTemplateItemValue::VT_Type && !value.mTypeValue.isVar())
			error_invalid_params("You should to use variable types in template");

		if (!value.mReplacementName.empty())
		{
			if (value.mItemType != ScTemplateItemValue::VT_Replace)
				mReplacements[value.mReplacementName] = replPos + i;

			/* Store type there, if replacement for any type.
			* That allows to use it before original type will processed
			*/
			size_t const constrIdx = replPos / 3;
			check_expr(constrIdx < mConstructions.size());
			ScTemplateItemValue const & valueType = mConstructions[constrIdx].mValues[i];

			if (valueType.isType())
				value.mTypeValue = valueType.mTypeValue;
		}
	}

	mIsCacheValid = false;

	return *this;
}

ScTemplate & ScTemplate::tripleWithRelation(ScTemplateItemValue const & param1, ScTemplateItemValue const & param2, ScTemplateItemValue const & param3,
	ScTemplateItemValue const & param4, ScTemplateItemValue const & param5)
{
	size_t const replPos = mConstructions.size() * 3;
	
	ScTemplateItemValue edgeCommonItem = param2;

	// check if relation edge has replacement
	if (edgeCommonItem.mReplacementName.empty())
	{
		std::stringstream ss;
		ss << "_repl_" << replPos + 1;
		edgeCommonItem.mReplacementName = ss.str();
	}

	triple(param1, edgeCommonItem, param3);
	triple(param5, param4, edgeCommonItem.mReplacementName.c_str());

	return *this;
}