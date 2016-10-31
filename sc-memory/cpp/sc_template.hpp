/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_types.hpp"
#include "sc_addr.hpp"
#include "sc_utils.hpp"


#define SC_REPL(x) (char const *)(x)

struct ScTemplateItemValue
{
	typedef enum
	{
		VT_None,
		VT_Type,
		VT_Addr,
		VT_Replace
	} eType;

	ScTemplateItemValue()
	{
		mItemType = VT_None;
	}

	ScTemplateItemValue(ScAddr const & addr, char const * replName = 0)
	{
		setAddr(addr, replName);
	}

	ScTemplateItemValue(ScType const & type, char const * replName = 0)
	{
		setType(type, replName);
	}

	ScTemplateItemValue(char const * name)
	{
		setReplacement(name);
	}

	ScTemplateItemValue(std::string const & name)
	{
		setReplacement(name.c_str());
	}

	inline bool isAddr() const
	{
		return (mItemType == VT_Addr);
	}

	inline bool isReplacement() const
	{
		return (mItemType == VT_Replace);
	}

	inline bool isType() const
	{
		return (mItemType == VT_Type);
	}

	inline bool isFixed() const
	{
		return (isAddr() || isReplacement());
	}

	inline bool isAssign() const
	{
		return (mItemType == VT_Type);
	}

	inline bool isItemType(eType type) const
	{
		return (mItemType == type);
	}

	void setAddr(ScAddr const & addr, char const * replName = 0)
	{
		mItemType = VT_Addr;
		mAddrValue = addr;
		if (replName)
			mReplacementName = replName;
	}

	void setType(ScType const & type, char const * replName = 0)
	{
		mItemType = VT_Type;
		mTypeValue = type;
		if (replName)
			mReplacementName = replName;
	}

	void setReplacement(char const * name)
	{
		mItemType = VT_Replace;
		if (name)
			mReplacementName = name;
	}

	SC_DEPRECATED(0.3.0, "Use ScTemplateItemValue::isFixed instead")
	bool canBeAddr() const
    {
        return mItemType == VT_Addr || mItemType == VT_Replace;
    }
	
	SC_DEPRECATED(0.3.0, "Use ScTemplateItemValue::isAssign instead")
    bool canBeType() const
    {
        return mItemType == VT_Type;
    }

	eType mItemType;

	ScAddr mAddrValue;
	ScType mTypeValue;
	std::string mReplacementName;
};

class ScTemplateConstr3
{
	friend class ScTemplate;
public:
	ScTemplateConstr3(ScTemplateItemValue const & param1, ScTemplateItemValue const & param2, ScTemplateItemValue const & param3, size_t idx)
		: mIndex(idx)
	{
		mValues[0] = param1;
		mValues[1] = param2;
		mValues[2] = param3;
	}

	ScTemplateItemValue const * getValues() const
	{
		return &(mValues[0]);
	}

	size_t countFixed() const
	{
		return countCommonT([](ScTemplateItemValue const & value) {
			return value.isFixed();
		});
	}

	size_t countAddrs() const
	{
		return countCommonT([](ScTemplateItemValue const & value) {
			return value.isAddr();
		});
	}
	size_t countReplacements() const
	{
		return countCommonT([](ScTemplateItemValue const & value) {
			return value.isReplacement();
		});
	}

	template <typename Func>
	size_t countCommonT(Func f) const
	{
		size_t result = 0;
		for (size_t i = 0; i < 3; ++i)
		{
			if (f(mValues[i]))
				++result;
		}
		return result;
	}

protected:
	ScTemplateItemValue mValues[3];
	/* Store original index in template. Because when perform search or generation 
	 * we sort triples in sutable for operation order.
	 * Used to construct result
	 */
	size_t mIndex;
};

template <typename Type>
ScTemplateItemValue operator >> (Type const & value, char const * replName)
{
	return ScTemplateItemValue(value, replName);
}

template <typename Type>
ScTemplateItemValue operator >> (Type const & value, std::string const & replName)
{
	return ScTemplateItemValue(value, replName.c_str());
}

class ScTemplateGenResult;
class ScTemplateSearchResult;

enum class ScTemplateResultCode : uint8_t
{
	Success = 0,
	InvalidParams = 1,
	InternalError = 2
};

/* Parameters for template generator.
 * Can be used to replace variables by values
 */
class ScTemplateGenParams
{
	friend class ScTemplateGenerator;

public:
	explicit ScTemplateGenParams()
	{
	}

	_SC_EXTERN ScTemplateGenParams & add(std::string const & varIdtf, ScAddr const & value)
	{
		assert(mValues.find(varIdtf) == mValues.end());
		mValues[varIdtf] = value;
		return *this;
	}

	_SC_EXTERN bool get(std::string const & varIdtf, ScAddr & outResult) const
	{
		tParamsMap::const_iterator it = mValues.find(varIdtf);
		if (it != mValues.end())
		{
			outResult = it->second;
			return true;
		}			

		return false;
	}

	_SC_EXTERN bool empty() const
	{
		return mValues.empty();
	}

	_SC_EXTERN static const ScTemplateGenParams Empty;

protected:
	typedef std::map<std::string, ScAddr> tParamsMap;
	tParamsMap mValues;
};

class ScTemplate final
{
	friend class ScMemoryContext;
	friend class ScTemplateSearch;
	friend class ScTemplateGenerator;
	friend class ScTemplateBuilder;

public:
	typedef std::map<std::string, size_t> tReplacementsMap;
	typedef std::vector<ScTemplateConstr3> tTemplateConstr3Vector;
	typedef std::vector<size_t> tProcessOrder;

	_SC_EXTERN explicit ScTemplate(size_t BufferedNum = 16);

	_SC_EXTERN ScTemplate & operator() (ScTemplateItemValue const & param1, ScTemplateItemValue const & param2, ScTemplateItemValue const & param3);

	_SC_EXTERN ScTemplate & operator() (ScTemplateItemValue const & param1, ScTemplateItemValue const & param2, ScTemplateItemValue const & param3,
		ScTemplateItemValue const & param4, ScTemplateItemValue const & param5);
	
	_SC_EXTERN void clear();

	bool isSearchCacheValid() const;
	bool isGenerateCacheValid() const;

	_SC_EXTERN bool hasReplacement(std::string const & repl) const;

	/** Add construction:
	 *          param2
	 * param1 ----------> param3
	 */
	_SC_EXTERN ScTemplate & triple(ScTemplateItemValue const & param1, ScTemplateItemValue const & param2, ScTemplateItemValue const & param3);

	/** Adds template:
	 *           param2
	 *	param1 ---------> param3
	 *             ^
	 *             |
	 *             | param4
	 *             |
	 *           param5
	 * Equal to two calls of triple, so this template add 6 items to result (NOT 5), to minimize
	 * possible abuse, use result name mapping, and get result by names
	 */
	_SC_EXTERN ScTemplate & tripleWithRelation(ScTemplateItemValue const & param1, ScTemplateItemValue const & param2, ScTemplateItemValue const & param3,
		ScTemplateItemValue const & param4, ScTemplateItemValue const & param5);

protected:
	// Begin: calls by memory context
	bool generate(ScMemoryContext & ctx, ScTemplateGenResult & result, ScTemplateGenParams const & params, ScTemplateResultCode * errorCode = nullptr) const;
    bool search(ScMemoryContext & ctx, ScTemplateSearchResult & result) const;
	bool searchInStruct(ScMemoryContext & ctx, ScAddr const & scStruct, ScTemplateSearchResult & result) const;
	
	// Builds template based on template in sc-memory
	bool fromScTemplate(ScMemoryContext & ctx, ScAddr const & scTemplateAddr);
	// End: calls by memory context
	
private:
	/** Generates node or link element in memory, depending on type. 
	 * If type isn't a node or link, then return empty addr
	 */
	ScAddr createNodeLink(ScMemoryContext & ctx, ScType const & type) const;

	/** Resolve ScAddr for specified ScTemplateItemValue.
	 * If ScAddr not resolved, then returns empty addr.
	 */
	ScAddr resolveAddr(ScMemoryContext & ctx, ScTemplateItemValue const & itemValue) const;

protected:
	// Store mapping of name to index in result vector
	tReplacementsMap mReplacements;
	// Store construction (triples)
	tTemplateConstr3Vector mConstructions;
	size_t mCurrentReplPos;

	/* Caches (used to prevent processing order update on each search/gen)
	 * Flag mIsCacheValid == false - request to update cache. We doesn't use two flags,
	 * because one enogh to get info, that triples changed. Can't use triple count instead of this flag.
	 * In some cases developer can remove on triple and add new one after that.
	 * Caches are mutable, to prevent changes of template in search and generation, they can asses just a cache.
	 * That because template passed into them by const reference.
	 */
	bool mIsCacheValid : 1;
	mutable tProcessOrder mSearchCachedOrder;
	mutable tProcessOrder mGenerateCachedOrder;
};



class ScTemplateGenResult
{
	friend class ScTemplateGenerator;
	friend class ScStruct;
    
public:
	ScTemplateGenResult() {	}

	ScAddr const & operator [] (std::string const & name) const
	{
		ScTemplate::tReplacementsMap::const_iterator it = mReplacements.find(name);
		if (it != mReplacements.end())
		{
			if (it->second >= mResult.size())
				error("Invalid replacement " + name);

			return mResult[it->second];
		}
		else
		{
			error("Can't find replacement " + name);
		}

		static ScAddr empty;
		return empty;
	}

	size_t getSize() const
	{
		return mResult.size();
	}

protected:
	tAddrVector mResult;

	ScTemplate::tReplacementsMap mReplacements;
};

class ScTemplateSearchResultItem
{
    friend class ScTemplateSearch;
    friend class ScTemplateSearchResult;

protected:
    explicit ScTemplateSearchResultItem(tAddrVector const * results, ScTemplate::tReplacementsMap const * replacements) 
        : mResults(results)
        , mReplacements(replacements)
    {
    }

public:
    ScAddr const & operator[] (std::string const & name) const
    {
        ScTemplate::tReplacementsMap::const_iterator it = mReplacements->find(name);
        check_expr(it != mReplacements->end());
		check_expr(it->second < mResults->size());
        return (*mResults)[it->second];
    }

protected:
    tAddrVector const * mResults;
    ScTemplate::tReplacementsMap const * mReplacements;
};

class ScTemplateSearchResult
{
    friend class ScTemplateSearch;

public:
    inline size_t getSize() const
    {
        return mResults.size();
    }

	inline bool getResultItemSave(size_t idx, ScTemplateSearchResultItem & outItem) const
	{
		if (idx < mResults.size())
		{
			outItem.mReplacements = &mReplacements;
			outItem.mResults = &(mResults[idx]);
			return true;
		}

		return false;
	}

	inline ScTemplateSearchResultItem operator [] (size_t idx) const
	{
		check_expr(idx < mResults.size());
		return ScTemplateSearchResultItem(&(mResults[idx]), &mReplacements);
	}

	inline void clear()
	{
		mResults.clear();
		mReplacements.clear();
	}

protected:
    typedef std::vector<tAddrVector> tSearchResults;
    tSearchResults mResults;
    ScTemplate::tReplacementsMap mReplacements;
};
