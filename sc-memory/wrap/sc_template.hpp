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

    bool canBeAddr() const
    {
        return mItemType == VT_Addr || mItemType == VT_Replace;
    }

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
	ScTemplateConstr3(ScTemplateItemValue const & param1, ScTemplateItemValue const & param2, ScTemplateItemValue const & param3)
	{
		mValues[0] = param1;
		mValues[1] = param2;
		mValues[2] = param3;
	}

	ScTemplateItemValue const * getValues() const
	{
		return &(mValues[0]);
	}

protected:
	ScTemplateItemValue mValues[3];
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

class ScTemplate
{
	friend class ScMemoryContext;

public:
	typedef std::map<std::string, size_t> tReplacementsMap;
	typedef std::vector<ScTemplateConstr3> tTemplateConts3Vector;

	explicit ScTemplate(size_t BufferedNum = 16)
	{
		mConstructions.reserve(BufferedNum);
		mCurrentReplPos = 0;
	}

	void clear();

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
	tTemplateConts3Vector mConstructions;
	size_t mCurrentReplPos;
};



class ScTemplateGenResult
{
	friend class ScTemplateGenerator;
    
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

	ScAddr const & operator [] (size_t idx) const
	{
		check_expr(idx < mResult.size());
		return mResult[idx];
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
    ScAddr const & operator[] (size_t idx) const
    {
        check_expr(idx < mResults->size());
        return (*mResults)[idx];
    }

    ScAddr const & operator[] (std::string const & name) const
    {
        ScTemplate::tReplacementsMap::const_iterator it = mReplacements->find(name);
        check_expr(it != mReplacements->end());
        return (*this)[it->second];
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

    inline ScTemplateSearchResultItem getResult(size_t idx)
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
