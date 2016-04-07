/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_template.hpp"
#include "sc_memory.hpp"


class ScTemplateGenerator
{
public:
	ScTemplateGenerator(ScTemplate::tReplacementsMap const & replacements,
						ScTemplate::tTemplateConts3Vector const & constructions,
						ScMemoryContext & context)
		: mReplacements(replacements)
		, mConstructions(constructions)
		, mContext(context)
	{
	}

	bool operator() (ScTemplateGenResult & result)
	{
		result.mResult.resize(mConstructions.size() * 3);
		result.mReplacements = mReplacements;

		tAddrList createdElements;
		size_t resultIdx = 0;
		bool isError = false;

		for (ScTemplate::tTemplateConts3Vector::const_iterator it = mConstructions.begin(); it != mConstructions.end(); ++it)
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
			ScAddr const addr2 = resolveAddr(values[2], result.mResult);

			if (!addr1.isValid() || !addr2.isValid())
			{
				isError = true;
				break;
			}

			ScAddr const edge = mContext.createArc(*values[1].mTypeValue, addr1, addr2);
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
			return false;
		}

		return true;
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
		switch (itemValue.mItemType)
		{
		case ScTemplateItemValue::VT_Addr:
			return itemValue.mAddrValue;
		case ScTemplateItemValue::VT_Type:
			return createNodeLink(itemValue.mTypeValue);
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

private:
	ScTemplate::tReplacementsMap const & mReplacements;
	ScTemplate::tTemplateConts3Vector const & mConstructions;
	ScMemoryContext & mContext;
	tAddrList mCreatedElements;
};

class ScTemplateSearch
{
public:
    ScTemplateSearch(ScTemplate::tReplacementsMap const & replacements, ScTemplate::tTemplateConts3Vector const & constructions, ScMemoryContext & context)
        : mReplacements(replacements)
        , mConstructions(constructions)
        , mContext(context)
    {
    }

    ScAddr const & resolveAddr(ScTemplateItemValue const & value) const
    {
        switch (value.mItemType)
        {
        case ScTemplateItemValue::VT_Addr:
            return value.mAddrValue;

        case ScTemplateItemValue::VT_Replace:
        {
            ScTemplate::tReplacementsMap::const_iterator it = mReplacements.find(value.mReplacementName);
            check_expr(it != mReplacements.end());
            check_expr(it->second < mResultAddrs.size());
            return mResultAddrs[it->second];
        }
        
        default:
            break;
        };

        static ScAddr empty;
        return empty;
    }

    ScIterator3Ptr createIterator(ScTemplateConstr3 const & constr)
    {
        ScTemplateItemValue const * values = constr.getValues();

        ScTemplateItemValue const & value0 = values[0];
        ScTemplateItemValue const & value1 = values[1];
        ScTemplateItemValue const & value2 = values[2];

        check_expr(value1.mItemType == ScTemplateItemValue::VT_Type);

        if (value0.canBeAddr())
        {
            ScAddr const addrSrc = resolveAddr(value0);
            check_expr(addrSrc.isValid());
            if (value2.canBeAddr()) // F_A_F
            {
                ScAddr const addrTrg = resolveAddr(value2);
                check_expr(addrTrg.isValid());
                return mContext.iterator3(addrSrc, *value1.mTypeValue, addrTrg);
            }
            else // F_A_A
            {
                return mContext.iterator3(addrSrc, *value1.mTypeValue, *value2.mTypeValue);
            }
        }
        else if (value2.canBeAddr())
        {
            ScAddr const addrTrg = resolveAddr(value2);
            check_expr(addrTrg.isValid());
            return mContext.iterator3(*value0.mTypeValue, *value1.mTypeValue, addrTrg);
        }

        return ScIterator3Ptr();
    }

    void iteration(size_t constrIndex, ScTemplateSearchResult & result)
    {
        check_expr(constrIndex < mConstructions.size());
        size_t const finishIdx = mConstructions.size() - 1;
        size_t resultIdx = constrIndex * 3;

        ScTemplateConstr3 const & constr = mConstructions[constrIndex];
        ScIterator3Ptr const it3 = createIterator(constr);
        while (it3->next())
        {
            // do not make cycle for optimization issues (remove comparsion expresion)
            mResultAddrs[resultIdx] = it3->value(0);
            mResultAddrs[resultIdx + 1] = it3->value(1);
            mResultAddrs[resultIdx + 2] = it3->value(2);

            if (constrIndex == finishIdx)
            {
                result.mResults.push_back(mResultAddrs);
            }
            else
            {
                iteration(constrIndex + 1, result);
            }
        }
    }

    bool operator () (ScTemplateSearchResult & result)
    {
		result.clear();

        result.mReplacements = mReplacements;
        mResultAddrs.resize(calculateOneResultSize());

        iteration(0, result);

        return result.getSize() > 0;
    }

    size_t calculateOneResultSize() const
    {
        return mConstructions.size() * 3;
    }

private:
    ScTemplate::tReplacementsMap const & mReplacements;
    ScTemplate::tTemplateConts3Vector const & mConstructions;
    ScMemoryContext & mContext;

    tAddrVector mResultAddrs;
};



bool ScTemplate::generate(ScMemoryContext & ctx,  ScTemplateGenResult & result) const
{
	ScTemplateGenerator gen(mReplacements, mConstructions, ctx);
	return gen(result);
}

bool ScTemplate::search(ScMemoryContext & ctx, ScTemplateSearchResult & result) const
{
    ScTemplateSearch search(mReplacements, mConstructions, ctx);
    return search(result);
}