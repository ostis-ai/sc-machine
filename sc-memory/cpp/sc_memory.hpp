/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

extern "C"
{
#include "sc-memory/sc_memory_headers.h"
#include "sc-memory/sc_helper.h"
}

#include "sc_types.hpp"
#include "sc_addr.hpp"
#include "sc_iterator.hpp"
#include "sc_template.hpp"
#include "sc_event.hpp"

#include <list>
#include <string>

class ScMemoryContext;
class ScStream;

/// TODO: replace with ScType
#define SC_TYPE(__t__) ((sc_type)(__t__))

class ScMemory
{
	friend class ScMemoryContext;

public:
    //! Returns true, on memory initialized; otherwise returns false
    _SC_EXTERN static bool initialize(sc_memory_params const & params);
    _SC_EXTERN static void shutdown(bool saveState = true);

	_SC_EXTERN static void logMute();
	_SC_EXTERN static void logUnmute();
protected:

    static void registerContext(ScMemoryContext const * ctx);
    static void unregisterContext(ScMemoryContext const * ctx);
private:
    static bool hasMemoryContext(ScMemoryContext const * ctx);

private:
    static sc_memory_context * msGlobalContext;

    typedef std::list<ScMemoryContext const *> tMemoryContextList;
    static tMemoryContextList msContexts;
};

//! Class used to work with memory. It provides functions to create/erase elements
class ScMemoryContext
{
public:
    _SC_EXTERN explicit ScMemoryContext(sc_uint8 accessLevels = 0, std::string const & name = "");
	_SC_EXTERN ~ScMemoryContext();

	sc_memory_context const * operator * () const { return mContext; }
    sc_memory_context const * getRealContext() const { return mContext; }

    //! Call this function, when you request to destroy real memory context, before destructor calls for this object
    _SC_EXTERN void destroy();

    std::string const & getName() const { return mName; }

    _SC_EXTERN bool isValid() const;

    //! Check if element exists with specified addr
	_SC_EXTERN bool isElement(ScAddr const & addr) const;
    //! Erase element from sc-memory and returns true on success; otherwise returns false.
	_SC_EXTERN bool eraseElement(ScAddr const & addr);

	_SC_EXTERN ScAddr createNode(sc_type type);
	_SC_EXTERN ScAddr createLink();

	SC_DEPRECATED(0.3.0, "Use ScMemoryContext::createEdge instead.")
	_SC_EXTERN ScAddr createArc(sc_type type, ScAddr const & addrBeg, ScAddr const & addrEnd);

	_SC_EXTERN ScAddr createEdge(sc_type type, ScAddr const & addrBeg, ScAddr const & addrEnd);

    //! Returns type of sc-element. If there are any error, then returns 0
	_SC_EXTERN ScType getElementType(ScAddr const & addr) const;

    /*! Change subtype of sc-element (subtype & sc_type_element_mask == 0).
     * Return true, if there are no any errors; otherwise return false.
     */
	_SC_EXTERN bool setElementSubtype(ScAddr const & addr, sc_type subtype);

	_SC_EXTERN ScAddr getEdgeSource(ScAddr const & edgeAddr) const;
	_SC_EXTERN ScAddr getEdgeTarget(ScAddr const & edgeAddr) const;
	_SC_EXTERN bool getEdgeInfo(ScAddr const & edgeAddr, ScAddr & outSourceAddr, ScAddr & outTargetAddr) const;

	SC_DEPRECATED(0.3.0, "Use ScMemoryContext::getEdgeSource instead.")
	_SC_EXTERN ScAddr getArcBegin(ScAddr const & arcAddr) const;
	SC_DEPRECATED(0.3.0, "Use ScMemoryContext::getEdgeTarget instead.")
	_SC_EXTERN ScAddr getArcEnd(ScAddr const & arcAddr) const;

	_SC_EXTERN bool setLinkContent(ScAddr const & addr, ScStream const & stream);
	_SC_EXTERN bool getLinkContent(ScAddr const & addr, ScStream & stream);

    //! Returns true, if any links found
	_SC_EXTERN bool findLinksByContent(ScStream const & stream, tAddrList & found);

    //! Saves memory state
    _SC_EXTERN bool save();

    template <typename ParamType1, typename ParamType2, typename ParamType3, typename ParamType4, typename ParamType5>
    TIterator5<ParamType1, ParamType2, ParamType3, ParamType4, ParamType5> * iterator5(ParamType1 const & param1,
                                                                                       ParamType2 const & param2,
                                                                                       ParamType3 const & param3,
                                                                                       ParamType4 const & param4,
                                                                                       ParamType5 const & param5)
    {
        return new TIterator5<ParamType1, ParamType2, ParamType3, ParamType4, ParamType5>(*this, param1, param2, param3, param4, param5);
    }

    template <typename ParamType1, typename ParamType2, typename ParamType3>
    TIterator3<ParamType1, ParamType2, ParamType3> * iterator3(ParamType1 const & param1,
                                                               ParamType2 const & param2,
                                                               ParamType3 const & param3)
    {
        return new TIterator3<ParamType1, ParamType2, ParamType3>(*this, param1, param2, param3);
    }


	_SC_EXTERN bool helperResolveSystemIdtf(std::string const & sysIdtf, ScAddr & outAddr, bool bForceCreation = false);
	_SC_EXTERN bool helperSetSystemIdtf(std::string const & sysIdtf, ScAddr const & addr);
	_SC_EXTERN std::string helperGetSystemIdtf(ScAddr const & addr);
	_SC_EXTERN bool helperCheckArc(ScAddr const & begin, ScAddr end, sc_type arcType);
	_SC_EXTERN bool helperFindBySystemIdtf(std::string const & sysIdtf, ScAddr & outAddr);
	_SC_EXTERN bool helperGenTemplate(ScTemplate const & templ, ScTemplateGenResult & result, ScTemplateGenParams const & params = ScTemplateGenParams::Empty, ScTemplateResultCode * resultCode = nullptr);
    _SC_EXTERN bool helperSearchTemplate(ScTemplate const & templ, ScTemplateSearchResult & result);
	_SC_EXTERN bool helperSearchTemplateInStruct(ScTemplate const & templ, ScAddr const & scStruct, ScTemplateSearchResult & result);
	_SC_EXTERN bool helperBuildTemplate(ScTemplate & templ, ScAddr const & templAddr);

private:
    // Disable object copying
	ScMemoryContext(ScMemoryContext const & other) {}
	ScMemoryContext & operator = (ScMemoryContext const & other) { return *this; }

private:
    sc_memory_context * mContext;
    std::string mName;
};


