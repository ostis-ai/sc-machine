/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_template.hpp"
#include "sc_memory.hpp"

#include <algorithm>


class ScTemplateSearch
{
public:
    ScTemplateSearch(ScTemplate const & templ,
					 ScMemoryContext & context,
					 ScAddr const & scStruct)
		: mTemplate(templ)
        , mContext(context)
		, mScStruct(scStruct)
    {
		updateSearchCache();
    }

	void updateSearchCache()
	{
		if (!mTemplate.isSearchCacheValid())
		{
			// update it
			auto const & triples = mTemplate.mConstructions;
			ScTemplate::tProcessOrder & cache = mTemplate.mSearchCachedOrder;
			cache.resize(mTemplate.mConstructions.size());
			size_t i = 0;
			for (size_t & v : cache)
				v = i++;

			/* Main idea of this stage, to build an order where iterators with maximum number of 
			 * fixed elements will runs first. This allow to minimize number of iterations. Also we can't 
			 * start search from template a_a_a, or any template where all fixed elements are remplacements
			 */
			std::sort(cache.begin(), cache.end(), [&triples](size_t a, size_t b) {
				ScTemplateConstr3 const & aTriple = triples[a];
				ScTemplateConstr3 const & bTriple = triples[b];

				// compare by addrs arguments count
				size_t const aAddrCount = aTriple.countAddrs();
				size_t const bAddrCount = bTriple.countAddrs();

				if (aAddrCount != bAddrCount)
					return (aAddrCount > bAddrCount);

				// compare by fixed arguments count
				size_t const aFCount = aTriple.countFixed();
				size_t const bFCount = bTriple.countFixed();

				if (aFCount != bFCount)
					return (aFCount > bFCount);

				// compare by replacements count
				size_t const aRCount = aTriple.countReplacements();
				size_t const bRCount = bTriple.countReplacements();

				if (aRCount != bRCount)
					return (aRCount > bRCount);

				return false;
			});
		}
	}

    ScAddr const & resolveAddr(ScTemplateItemValue const & value) const
    {
        switch (value.mItemType)
        {
        case ScTemplateItemValue::VT_Addr:
            return value.mAddrValue;

        case ScTemplateItemValue::VT_Replace:
        {
            auto it = mTemplate.mReplacements.find(value.mReplacementName);
            check_expr(it != mTemplate.mReplacements.end());
            check_expr(it->second < mResultAddrs.size());
            return mResultAddrs[it->second];
        }

		case ScTemplateItemValue::VT_Type:
		{
			if (!value.mReplacementName.empty())
			{
				auto it = mTemplate.mReplacements.find(value.mReplacementName);
				check_expr(it != mTemplate.mReplacements.end());
				check_expr(it->second < mResultAddrs.size());
				return mResultAddrs[it->second];
			}
			break;
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

		ScAddr const addr0 = resolveAddr(value0);
		ScAddr const addr1 = resolveAddr(value1);
		ScAddr const addr2 = resolveAddr(value2);

		if (addr0.isValid())
		{
			if (addr2.isValid()) // F_A_F
			{
				return mContext.iterator3(addr0, *value1.mTypeValue.upConstType(), addr2);
			}
			else // F_A_A
			{
				return mContext.iterator3(addr0, *value1.mTypeValue.upConstType(), *value2.mTypeValue.upConstType());
			}
		}
		else if (addr2.isValid()) // A_A_F
		{
			return mContext.iterator3(*value0.mTypeValue.upConstType(), *value1.mTypeValue.upConstType(), addr2);
		}
		else if (addr1.isValid()) // A_F_A
		{
			return mContext.iterator3(*value0.mTypeValue.upConstType(), addr1, *value2.mTypeValue.upConstType());
		}
		else // unknown iterator type
		{
			error("Unknown iterator type");
		}

        return ScIterator3Ptr();
    }

	bool checkInStruct(ScAddr const & addr)
	{
		tStructCache::const_iterator it = mStructCache.find(addr);
		if (it != mStructCache.end())
			return true;
		
		if (mContext.helperCheckArc(mScStruct, addr, sc_type_arc_pos_const_perm))
		{
			mStructCache.insert(addr);
			return true;
		}

		return false;
	}

	void refReplacement(ScTemplateItemValue const & v, ScAddr const & addr)
	{
		if (!v.mReplacementName.empty())
		{
			auto it = mTemplate.mReplacements.find(v.mReplacementName);
			check_expr(it != mTemplate.mReplacements.end());		

			mResultAddrs[it->second] = addr;
			mReplRefs[it->second]++;
		}
	}

	void unrefReplacement(ScTemplateItemValue const & v)
	{
		if (!v.mReplacementName.empty())
		{
			auto it = mTemplate.mReplacements.find(v.mReplacementName);
			check_expr(it != mTemplate.mReplacements.end());
			
			mReplRefs[it->second]--;
			if (mReplRefs[it->second] == 0)
				mResultAddrs[it->second].reset();
		}
	}

	void iteration(size_t orderIndex, ScTemplateSearchResult & result)
    {
		size_t const constrIndex = mTemplate.mSearchCachedOrder[orderIndex];

        check_expr(constrIndex < mTemplate.mConstructions.size());
		size_t const finishIdx = mTemplate.mConstructions.size() - 1;
        size_t resultIdx = constrIndex * 3;
		
		/// TODO: prevent recursive search and make test for that case

		ScTemplateConstr3 const & constr = mTemplate.mConstructions[constrIndex];
		ScTemplateItemValue const * values = constr.getValues();
        ScIterator3Ptr const it3 = createIterator(constr);
        while (it3->next())
        {
			/// check if search in structure
			if (mScStruct.isValid())
			{
				if (!checkInStruct(it3->value(0)) ||
					!checkInStruct(it3->value(1)) ||
					!checkInStruct(it3->value(2)))
				{
					continue;
				}
			}

            // do not make cycle for optimization issues (remove comparsion expresion)
            mResultAddrs[resultIdx] = it3->value(0);
            mResultAddrs[resultIdx + 1] = it3->value(1);
            mResultAddrs[resultIdx + 2] = it3->value(2);

			refReplacement(values[0], it3->value(0));
			refReplacement(values[1], it3->value(1));
			refReplacement(values[2], it3->value(2));			

			if (orderIndex == finishIdx)
            {
                result.mResults.push_back(mResultAddrs);
            }
            else
            {
				iteration(orderIndex + 1, result);
            }

			unrefReplacement(values[0]);
			unrefReplacement(values[1]);
			unrefReplacement(values[2]);
        }
    }

    bool operator () (ScTemplateSearchResult & result)
    {
		result.clear();

		result.mReplacements = mTemplate.mReplacements;
        mResultAddrs.resize(calculateOneResultSize());
		mReplRefs.resize(mResultAddrs.size(), 0);

        iteration(0, result);

        return result.getSize() > 0;
    }

    size_t calculateOneResultSize() const
    {
		return mTemplate.mConstructions.size() * 3;
    }

private:
	ScTemplate const & mTemplate;
    ScMemoryContext & mContext;
	ScAddr const & mScStruct;

	typedef std::unordered_set<ScAddr, ScAddrHashFunc<uint32_t>> tStructCache;
	tStructCache mStructCache;
	
    tAddrVector mResultAddrs;
	typedef std::vector<uint32_t> tReplRefs;
	tReplRefs mReplRefs;
};

bool ScTemplate::search(ScMemoryContext & ctx, ScTemplateSearchResult & result) const
{
    ScTemplateSearch search(*this, ctx, ScAddr());
    return search(result);
}

bool ScTemplate::searchInStruct(ScMemoryContext & ctx, ScAddr const & scStruct, ScTemplateSearchResult & result) const
{
	ScTemplateSearch search(*this, ctx, scStruct);
	return search(result);
}
