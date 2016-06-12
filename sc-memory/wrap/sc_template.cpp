/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_template.hpp"
#include "sc_memory.hpp"

#include <algorithm>

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

class ScTemplateBuilder
{
	friend class ScTemplate;

	typedef std::map<tRealAddr, size_t, tRealAddrLess> tAddrToIndexMap;

	struct ObjectInfo
	{
		ScAddr mAddr;
		ScType mType;
		std::string mSysIdtf;

		ObjectInfo(ScAddr const & inAddr, ScType const & inType, std::string const & inSysIdtf)
			: mAddr(inAddr)
			, mType(inType)
			, mSysIdtf(inSysIdtf)
		{
		}
	};

	struct EdgeInfo
	{
		size_t mSourceObject;
		size_t mTargetObject;
		ScType mType;
		size_t mEdgeIndex;

		EdgeInfo()
			: mSourceObject(-1)
			, mTargetObject(-1)
			, mType(0)
			, mEdgeIndex(-1)
		{
		}

		EdgeInfo(size_t inSourceObject, size_t inTargetObject, ScType const & inType, size_t inEdgeIndex)
			: mSourceObject(inSourceObject)
			, mTargetObject(inTargetObject)
			, mType(inType)
			, mEdgeIndex(inEdgeIndex)
		{
		}
	};

	struct EdgeCompareLess
	{
		EdgeCompareLess(ScTemplateBuilder * builder)
			: mBuilder(builder)
		{
			check_expr(mBuilder);
		}

		bool _isEdgeDependOnOther(EdgeInfo const & left, EdgeInfo const & right) const
		{
			check_expr(left.mTargetObject < mBuilder->mObjects.size());

			ObjectInfo & trgObj = mBuilder->mObjects[left.mTargetObject];
			if (trgObj.mType.isEdge())
			{
				tAddrToIndexMap::const_iterator it = mBuilder->mEdgeAddrToEdgeIndex.find(*trgObj.mAddr);
				check_expr(it != mBuilder->mEdgeAddrToEdgeIndex.end());
				check_expr(it->second < mBuilder->mEdges.size());

				return _isEdgeDependOnOther(mBuilder->mEdges[it->second], right);
			}

			return false;
		}

		bool operator() (EdgeInfo const & left, EdgeInfo const & right) const
		{
			ObjectInfo const & leftSrc = mBuilder->mObjects[left.mSourceObject];
			ObjectInfo const & leftTrg = mBuilder->mObjects[left.mTargetObject];

			ObjectInfo const & rightSrc = mBuilder->mObjects[right.mSourceObject];
			ObjectInfo const & rightTrg = mBuilder->mObjects[right.mTargetObject];

			size_t const leftConstNum = leftSrc.mType.bitAnd(sc_type_const) + leftTrg.mType.bitAnd(sc_type_const);
			size_t const rightConstNum = rightSrc.mType.bitAnd(sc_type_const) + rightTrg.mType.bitAnd(sc_type_const);

			// compare by constants count (more constants - better)
			if (leftConstNum > rightConstNum)
				return true;
			
			if (leftConstNum < rightConstNum)
				return false;

			// compare by end element type (edge - not good)
			bool const isLeftEdge = leftTrg.mType.isEdge();
			bool const isRightEdge = rightTrg.mType.isEdge();
			if (!isLeftEdge && isRightEdge)
				return true;
			if (isLeftEdge && !isRightEdge)
				return false;

			// check if edge depend on other edge
			if (_isEdgeDependOnOther(left, right))
				return true;

			return (left.mEdgeIndex < right.mEdgeIndex);
		}

	protected:
		ScTemplateBuilder * mBuilder;
	};

protected:
	ScTemplateBuilder(ScAddr const & inScTemplateAddr, ScMemoryContext & inCtx)
		: mScTemplateAddr(inScTemplateAddr)
		, mContext(inCtx)
	{
	}

	bool operator() (ScTemplate * inTemplate)
	{
		/// TODO: lock structure
		/// TODO: check for duplicates

		inTemplate->clear();

		// first of all iterate all objects in template
		size_t index = 0;
		mObjects.clear();
		mObjects.reserve(128);
		ScIterator3Ptr iterObj = mContext.iterator3(mScTemplateAddr, SC_TYPE(sc_type_arc_pos_const_perm), SC_TYPE(0));
		while (iterObj->next())
		{
			ScAddr const addr = iterObj->value(2);
			ScType const type = mContext.getElementType(addr);
			std::string idtf = mContext.helperGetSystemIdtf(addr);
			
			if (idtf.empty())
			{
				std::stringstream ss;
				ss << "obj_" << index++;
				idtf = ss.str();
			}

			size_t const index = mObjects.size();
			if (type.isConst())
			{
				mConstObjectIndicies.push_back(index);
			}
			else
			{
				mVarObjectIndicies.push_back(index);
			}

			if (type.isEdge())
				mEdgeIndicies.push_back(index);

			mAddrToIndex[*addr] = index;
			mObjects.push_back(ObjectInfo(addr, type, idtf));		
		}

		// iterate all edges, to detect begin and end elements
		{
			size_t const edgeNum = mEdgeIndicies.size();
			mEdges.clear();
			mEdges.resize(edgeNum);
			for (size_t i = 0; i < edgeNum; ++i)
			{
				ObjectInfo const & object = mObjects[mEdgeIndicies[i]];
				check_expr(object.mType.isEdge());

				ScAddr edgeAddr = object.mAddr;
				EdgeInfo & edge = mEdges[i];

				check_expr(edgeAddr.isValid());

				ScAddr sourceAddr = mContext.getArcBegin(edgeAddr);
				ScAddr targetAddr = mContext.getArcEnd(edgeAddr);
				check_expr(sourceAddr.isValid() && targetAddr.isValid());

				tAddrToIndexMap::const_iterator it = mAddrToIndex.find(*sourceAddr);
				if (it == mAddrToIndex.end())
					return false;	/// TODO: provide error code

				edge.mSourceObject = it->second;

				it = mAddrToIndex.find(*targetAddr);
				if (it == mAddrToIndex.end())
					return false;	/// TODO: provide error code

				edge.mTargetObject = it->second;
				edge.mType = object.mType;
				edge.mEdgeIndex = i;

				mEdgeAddrToEdgeIndex[*edgeAddr] = edge.mEdgeIndex;
			}
		}

		// sort edges by next rules:
		// - more constants at the begin
		// - more nodes at the begin
		/// TODO: support correct order of edges for generation
		{
			EdgeCompareLess compare(this);
			std::sort(mEdges.begin(), mEdges.end(), compare);
		}

		// build template
		{
			size_t const edgeNum = mEdges.size();
			for (size_t i = 0; i < edgeNum; ++i)
			{
				EdgeInfo const & edge = mEdges[i];
				check_expr(edge.mType.isVar());

				ObjectInfo const & edgeObj = mObjects[mEdgeIndicies[edge.mEdgeIndex]];
				ObjectInfo const & src = mObjects[edge.mSourceObject];
				ObjectInfo const & trg = mObjects[edge.mTargetObject];
				
				/// TODO: replacements
				if (src.mType.isConst())
				{
					if (trg.mType.isConst())	// F_A_F
					{
						inTemplate->triple(src.mAddr >> src.mSysIdtf, edge.mType.asConst() >> edgeObj.mSysIdtf, trg.mAddr >> trg.mSysIdtf);
					}
					else
					{
						if (inTemplate->hasReplacement(trg.mSysIdtf)) // F_A_F
						{
							inTemplate->triple(src.mAddr >> src.mSysIdtf, edge.mType.asConst() >> edgeObj.mSysIdtf, trg.mSysIdtf);
						}
						else // F_A_A
						{
							inTemplate->triple(src.mAddr >> src.mSysIdtf, edge.mType.asConst() >> edgeObj.mSysIdtf, trg.mType.asConst() >> trg.mSysIdtf);
						}
					}
				}
				else if (trg.mType.isConst())
				{
					if (inTemplate->hasReplacement(src.mSysIdtf)) // F_A_F
					{
						inTemplate->triple(src.mSysIdtf, edge.mType.asConst() >> edgeObj.mSysIdtf, trg.mAddr >> trg.mSysIdtf);
					}
					else // A_A_F
					{
						inTemplate->triple(src.mType.asConst() >> src.mSysIdtf, edge.mType.asConst() >> edgeObj.mSysIdtf, trg.mAddr >> trg.mSysIdtf);
					}
				}
				else
				{
					bool const srcRepl = inTemplate->hasReplacement(src.mSysIdtf);
					bool const trgRepl = inTemplate->hasReplacement(trg.mSysIdtf);

					if (srcRepl && trgRepl) // F_A_F
					{
						inTemplate->triple(src.mSysIdtf, edge.mType.asConst() >> edgeObj.mSysIdtf, trg.mSysIdtf);
					}
					else if (!srcRepl && trgRepl) // A_A_F
					{
						inTemplate->triple(src.mType.asConst() >> src.mSysIdtf, edge.mType.asConst() >> edgeObj.mSysIdtf, trg.mSysIdtf);
					} 
					else if (srcRepl && !trgRepl) // F_A_A
					{
						inTemplate->triple(src.mSysIdtf, edge.mType.asConst() >> edgeObj.mSysIdtf, trg.mType >> trg.mSysIdtf);
					}
					else
					{
						error("Invalid state of template");
					}
				}
			}
		}

		return true;
	}

protected:
	ScAddr mScTemplateAddr;
	ScMemoryContext & mContext;
	
	// all objects in template
	std::vector<ObjectInfo> mObjects;
	std::vector<EdgeInfo> mEdges;
	tAddrToIndexMap mAddrToIndex;
	tAddrToIndexMap mEdgeAddrToEdgeIndex;

	// indecies of objects by type
	std::vector<size_t> mConstObjectIndicies;
	std::vector<size_t> mVarObjectIndicies;
	std::vector<size_t> mEdgeIndicies;
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

bool ScTemplate::fromScTemplate(ScMemoryContext & ctx, ScAddr const & scTemplateAddr)
{
	ScTemplateBuilder builder(scTemplateAddr, ctx);
	return builder(this);
}

void ScTemplate::clear()
{
	mConstructions.clear();
	mReplacements.clear();
	mCurrentReplPos = 0;
}

bool ScTemplate::hasReplacement(std::string const & repl) const
{
	return (mReplacements.find(repl) != mReplacements.end());
}

ScTemplate & ScTemplate::triple(ScTemplateItemValue const & param1, ScTemplateItemValue const & param2, ScTemplateItemValue const & param3)
{
	size_t const replPos = mConstructions.size() * 3;
	mConstructions.push_back(ScTemplateConstr3(param1, param2, param3)); // TODO: replace with emplace_back in c++11

	ScTemplateConstr3 const & constr3 = mConstructions.back();
	for (size_t i = 0; i < 3; ++i)
	{
		ScTemplateItemValue const & value = constr3.mValues[i];
		if (value.mItemType != ScTemplateItemValue::VT_Replace && !value.mReplacementName.empty())
		{
			mReplacements[value.mReplacementName] = replPos + i;
		}
	}

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