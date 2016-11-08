/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_template.hpp"
#include "sc_memory.hpp"

#include <algorithm>

const ScTemplateGenParams ScTemplateGenParams::Empty = ScTemplateGenParams();

class ScTemplateGenerator
{
public:
	ScTemplateGenerator(ScTemplate::tReplacementsMap const & replacements,
						ScTemplate::tTemplateConstr3Vector const & constructions,
						ScTemplateGenParams const & params,
						ScMemoryContext & context)
		: mReplacements(replacements)
		, mConstructions(constructions)
		, mParams(params)
		, mContext(context)
	{
		// check if it valid
		for (auto constr : mConstructions)
		{
			auto values = constr.getValues();
			if (values[1].isFixed())
				error_invalid_params("You can't use fixed value for edge in triple for template generation")
			
			auto checkEdge = [](ScTemplateItemValue const & value)
			{
				if (value.isAssign() && (!value.isType() || value.mTypeValue.isEdge()))
					error_invalid_params("You can't use edges as a source/target element in triple for generation")
			};

			checkEdge(values[0]);
			checkEdge(values[2]);
		}
	}

	ScTemplateResultCode operator() (ScTemplateGenResult & result)
	{
		if (!checkParams())
			return ScTemplateResultCode::InvalidParams;	/// TODO: Provide error

		result.mResult.resize(mConstructions.size() * 3);
		result.mReplacements = mReplacements;

		tAddrList createdElements;
		size_t resultIdx = 0;
		bool isError = false;

		for (auto it = mConstructions.begin(); it != mConstructions.end(); ++it)
		{
			ScTemplateConstr3 const & item = *it;
			ScTemplateItemValue const * values = item.getValues();

			// check that the third argument isn't a command to generate edge
			check_expr(!(values[2].mItemType == ScTemplateItemValue::VT_Type && values[2].mTypeValue.isEdge()));
			// check that second command is to generate edge
			check_expr(values[1].mItemType == ScTemplateItemValue::VT_Type && values[1].mTypeValue.isEdge());
			// the second item couldn't be a replacement
			check_expr(values[1].mItemType != ScTemplateItemValue::VT_Replace);

			ScAddr const addr1 = resolveAddr(values[0], result.mResult);
			check_expr(addr1.isValid());
			ScAddr const addr2 = resolveAddr(values[2], result.mResult);
			check_expr(addr2.isValid());

			if (!addr1.isValid() || !addr2.isValid())
			{
				isError = true;
				break;
			}

			ScAddr const edge = mContext.createEdge(*values[1].mTypeValue.upConstType(), addr1, addr2);
			if (!edge.isValid())
			{
				isError = true;
				break;
			}

			result.mResult[resultIdx++] = addr1;
			result.mResult[resultIdx++] = edge;
			result.mResult[resultIdx++] = addr2;
		}

		if (isError)
		{
			cleanupCreated();
			return ScTemplateResultCode::InternalError;
		}

		return ScTemplateResultCode::Success;
	}

	ScAddr createNodeLink(ScType const & type)
	{
		ScAddr addr;
		if (type.isNode())
		{
			addr = mContext.createNode(*type);
		}
		else if (type.isLink())
		{
			addr = mContext.createLink();
		}

		if (addr.isValid())
			mCreatedElements.push_back(addr);

		return addr;
	}

	ScAddr resolveAddr(ScTemplateItemValue const & itemValue, tAddrVector const & resultAddrs)
	{
		/// TODO: improve speed, because not all time we need to replace by params
		// replace by value from params
		if (!mParams.empty() && !itemValue.mReplacementName.empty())
		{
			ScAddr result;
			if (mParams.get(itemValue.mReplacementName, result))
				return result;
		}

		switch (itemValue.mItemType)
		{
		case ScTemplateItemValue::VT_Addr:
			return itemValue.mAddrValue;
		case ScTemplateItemValue::VT_Type:
			return createNodeLink(itemValue.mTypeValue.upConstType());
		case ScTemplateItemValue::VT_Replace:
		{
			ScTemplate::tReplacementsMap::const_iterator it = mReplacements.find(itemValue.mReplacementName);
			if (it != mReplacements.end())
			{
				check_expr(it->second < resultAddrs.size());
				return resultAddrs[it->second];
			}
		}
		default:
			break;
		}

		return ScAddr();
	}

	void cleanupCreated()
	{
		for (tAddrList::iterator it = mCreatedElements.begin(); it != mCreatedElements.end(); ++it)
			mContext.eraseElement(*it);
		mCreatedElements.clear();
	}

	bool checkParams() const
	{
		ScTemplateGenParams::tParamsMap::const_iterator it;
		for (it = mParams.mValues.begin(); it != mParams.mValues.end(); ++it)
		{
			ScTemplate::tReplacementsMap::const_iterator const itRepl = mReplacements.find(it->first);

			if (itRepl == mReplacements.end())
				return false;

			ScTemplateConstr3 const & constr = mConstructions[itRepl->second / 3];
			ScType const & itemType = constr.getValues()[itRepl->second % 3].mTypeValue;
			/// TODO: check subtype of objects. Can't replace tuple with no tuple object
			if (!itemType.isVar() || itemType.isEdge())
				return false;
		}

		return true;
	}

private:
	ScTemplate::tReplacementsMap const & mReplacements;
	ScTemplate::tTemplateConstr3Vector const & mConstructions;
	ScTemplateGenParams const & mParams;
	ScMemoryContext & mContext;
	tAddrList mCreatedElements;
};


bool ScTemplate::generate(ScMemoryContext & ctx, ScTemplateGenResult & result, ScTemplateGenParams const & params, ScTemplateResultCode * errorCode) const
{
	ScTemplateGenerator gen(mReplacements, mConstructions, params, ctx);
	ScTemplateResultCode resultCode = gen(result);

	if (errorCode)
		*errorCode = resultCode;

	return (resultCode == ScTemplateResultCode::Success);
}

