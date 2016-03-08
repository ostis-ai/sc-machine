/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_memory.hpp"
#include "sc_utils.hpp"
#include "sc_stream.hpp"
#include <assert.h>

#include <iostream>
#include <sstream>

#define SC_BOOL(x) (x) ? SC_TRUE : SC_FALSE

unsigned int gContextGounter;

// ------------------

sc_memory_context * ScMemory::msGlobalContext = 0;
ScMemory::tMemoryContextList ScMemory::msContexts;

bool ScMemory::initialize(sc_memory_params const & params)
{
    gContextGounter = 0;

    msGlobalContext = sc_memory_initialize(&params);
    return msGlobalContext != null_ptr;
}

void ScMemory::shutdown(bool saveState /* = true */)
{
    if (msContexts.size() > 0)
    {
        std::stringstream description;
        description << "There are " << msContexts.size() << " contexts, wan't destroyed, before Memory::shutdown:";
        tMemoryContextList::const_iterator it = msContexts.begin();
        for (; it != msContexts.end(); ++it)
            description << "\t\n" << (*it)->getName();

        error(description.str());
    }

    sc_memory_shutdown(SC_BOOL(saveState));
    msGlobalContext = 0;
}

void ScMemory::registerContext(ScMemoryContext const * ctx)
{
    assert(!hasMemoryContext(ctx));
    msContexts.push_back(ctx);
}

void ScMemory::unregisterContext(ScMemoryContext const * ctx)
{
    assert(hasMemoryContext(ctx));
    tMemoryContextList::iterator it = msContexts.begin();
    for (; it != msContexts.end(); ++it)
    {
        if (*it == ctx)
        {
            msContexts.erase(it);
            return;
        }
    }
}

bool ScMemory::hasMemoryContext(ScMemoryContext const * ctx)
{
    tMemoryContextList::const_iterator it = msContexts.begin();
    for (; it != msContexts.end(); ++it)
    {
        if (*it == ctx)
            return true;
    }
    return false;
}

// ---------------

ScMemoryContext::ScMemoryContext(sc_uint8 accessLevels /* = 0 */, std::string const & name /* = "" */)
    : mContext(0)
{
    mContext = sc_memory_context_new(accessLevels);
    if (name.empty())
    {
        std::stringstream ss;
        ss << "Context_" << gContextGounter;
        mName = ss.str();
    }
    else
        mName = name;

	ScMemory::registerContext(this);
}

ScMemoryContext::~ScMemoryContext()
{
    destroy();
}

void ScMemoryContext::destroy()
{
    if (mContext)
    {
		ScMemory::unregisterContext(this);

        sc_memory_context_free(mContext);
        mContext = 0;
    }
}

bool ScMemoryContext::isValid() const
{
    return mContext != 0;
}

bool ScMemoryContext::isElement(ScAddr const & addr) const
{
    check_expr(isValid());
    return (sc_memory_is_element(mContext, addr.mRealAddr) == SC_TRUE);
}

bool ScMemoryContext::eraseElement(ScAddr const & addr)
{
    check_expr(isValid());
    return sc_memory_element_free(mContext, addr.mRealAddr) == SC_RESULT_OK;
}

ScAddr ScMemoryContext::createNode(sc_type type)
{
    check_expr(isValid());
	return ScAddr(sc_memory_node_new(mContext, type));
}

ScAddr ScMemoryContext::createLink()
{
    check_expr(isValid());
	return ScAddr(sc_memory_link_new(mContext));
}

ScAddr ScMemoryContext::createArc(sc_type type, ScAddr const & addrBeg, ScAddr const & addrEnd)
{
    check_expr(isValid());
	return ScAddr(sc_memory_arc_new(mContext, type, addrBeg.mRealAddr, addrEnd.mRealAddr));
}

sc_type ScMemoryContext::getElementType(ScAddr const & addr) const
{
    check_expr(isValid());
    sc_type type = 0;
    return (sc_memory_get_element_type(mContext, addr.mRealAddr, &type) == SC_RESULT_OK) ? type : 0;
}

bool ScMemoryContext::setElementSubtype(ScAddr const & addr, sc_type subtype)
{
    check_expr(isValid());
    return sc_memory_change_element_subtype(mContext, addr.mRealAddr, subtype) == SC_RESULT_OK;
}

ScAddr ScMemoryContext::getArcBegin(ScAddr const & arcAddr) const
{
    check_expr(isValid());
	ScAddr addr;
    if (sc_memory_get_arc_begin(mContext, arcAddr.mRealAddr, &addr.mRealAddr) != SC_RESULT_OK)
        addr.reset();

    return addr;
}

ScAddr ScMemoryContext::getArcEnd(ScAddr const & arcAddr) const
{
    check_expr(isValid());
	ScAddr addr;
    if (sc_memory_get_arc_end(mContext, arcAddr.mRealAddr, &addr.mRealAddr) != SC_RESULT_OK)
        addr.reset();

    return addr;
}

bool ScMemoryContext::setLinkContent(ScAddr const & addr, ScStream const & stream)
{
    check_expr(isValid());
    return sc_memory_set_link_content(mContext, addr.mRealAddr, stream.mStream) == SC_RESULT_OK;
}

bool ScMemoryContext::getLinkContent(ScAddr const & addr, ScStream & stream)
{
    check_expr(isValid());

    sc_stream * s = 0;
    if (sc_memory_get_link_content(mContext, addr.mRealAddr, &s) != SC_RESULT_OK)
    {
        stream.reset();
        return false;
    }

    stream.init(s);

    return stream.isValid();
}

bool ScMemoryContext::findLinksByContent(ScStream const & stream, tAddrList & found)
{
    check_expr(isValid());

    sc_addr * result = 0;
    sc_uint32 resultCount = 0;

    found.clear();
    if (sc_memory_find_links_with_content(mContext, stream.mStream, &result, &resultCount) != SC_RESULT_OK)
        return false;

    for (sc_uint32 i = 0; i < resultCount; ++i)
		found.push_back(ScAddr(result[i]));

	if (result)
		sc_memory_free_buff(result);

    return found.size() > 0;
}

bool ScMemoryContext::save()
{
    check_expr(isValid());
    return (sc_memory_save(mContext) == SC_RESULT_OK);
}

bool ScMemoryContext::helperResolveSystemIdtf(std::string const & sysIdtf, ScAddr & outAddr)
{
	check_expr(isValid());
	return (sc_helper_resolve_system_identifier(mContext, sysIdtf.c_str(), &outAddr.mRealAddr) == SC_TRUE);
}

bool ScMemoryContext::helperSetSystemIdtf(std::string const & sysIdtf, ScAddr const & addr)
{
	check_expr(isValid());
	return (sc_helper_set_system_identifier(mContext, addr.mRealAddr, sysIdtf.c_str(), (sc_uint32)sysIdtf.size()) == SC_RESULT_OK);
}

bool ScMemoryContext::helperCheckArc(ScAddr const & begin, ScAddr end, sc_type arcType)
{
	check_expr(isValid());
	return (sc_helper_check_arc(mContext, begin.mRealAddr, end.mRealAddr, arcType) == SC_RESULT_OK);
}

bool ScMemoryContext::helperGenTemplate(ScTemplate const & templ, ScTemplateGenResult & result)
{
	return templ.generate(*this, result);
}
