/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "translator.h"
#include "utils.h"


#define NREL_FORMAT_STR     "nrel_format"

iTranslator::tStringAddrMap iTranslator::msGlobalIdtfAddrs = iTranslator::tStringAddrMap();

iTranslator::iTranslator(sc_memory_context *context)
    : mContext(context)
{
}

iTranslator::~iTranslator()
{
}

bool iTranslator::translate(const TranslatorParams &params)
{
    mParams = params;
    return translateImpl();
}

void iTranslator::generateFormatInfo(sc_addr addr, const String &ext)
{
    String fmtStr = "format_" + ext;

    tStringAddrMap::iterator it = mSysIdtfAddrs.find(fmtStr);
    sc_addr fmt_addr;

    if (it != mSysIdtfAddrs.end())
    {
        fmt_addr = it->second;
    }
    else
    {
        // try to find by system identifier
        if (sc_helper_find_element_by_system_identifier(mContext, fmtStr.c_str(), (sc_uint32)fmtStr.size(), &fmt_addr) != SC_RESULT_OK)
        {
            fmt_addr = sc_memory_node_new(mContext, sc_type_node_class | sc_type_const);
            sc_helper_set_system_identifier(mContext, fmt_addr, fmtStr.c_str(), (sc_uint32)fmtStr.size());
            mSysIdtfAddrs[fmtStr] = fmt_addr;
        }
    }

    // try to find format relation
    sc_addr nrel_format_addr;
    String nrel_format_str = NREL_FORMAT_STR;
    it = mSysIdtfAddrs.find(nrel_format_str);
    if (it != mSysIdtfAddrs.end())
    {
        nrel_format_addr = it->second;
    }
    else
    {
        // try to find by system identifier
        if (sc_helper_find_element_by_system_identifier(mContext, nrel_format_str.c_str(), (sc_uint32)nrel_format_str.size(), &nrel_format_addr) != SC_RESULT_OK)
        {
            nrel_format_addr = sc_memory_node_new(mContext, sc_type_node_norole | sc_type_const);
            sc_helper_set_system_identifier(mContext, nrel_format_addr, nrel_format_str.c_str(), (sc_uint32)nrel_format_str.size());
            mSysIdtfAddrs[nrel_format_str] = nrel_format_addr;
        }
    }

    // connect sc-link with format
    sc_addr arc_addr = sc_memory_arc_new(mContext, sc_type_arc_common | sc_type_const, addr, fmt_addr);
    sc_memory_arc_new(mContext, sc_type_arc_pos_const_perm, nrel_format_addr, arc_addr);
}


iTranslator::eIdtfVisibility iTranslator::_getIdentifierVisibility(const String &idtf) const
{
    if (StringUtil::startsWith(idtf, "..", false))
        return IdtfLocal;
    else if (StringUtil::startsWith(idtf, ".", false))
        return IdtfGlobal;

    return IdtfSystem;
}

void iTranslator::appendScAddr(sc_addr addr, const String &idtf)
{
    if (idtf.empty())
        return;

    switch (_getIdentifierVisibility(idtf))
    {
    case IdtfSystem:
        assert(mSysIdtfAddrs.find(idtf) == mSysIdtfAddrs.end());
        mSysIdtfAddrs[idtf] = addr;
        break;

    case IdtfLocal:
        assert(mLocalIdtfAddrs.find(idtf) == mLocalIdtfAddrs.end());
        mLocalIdtfAddrs[idtf] = addr;
        break;

    case IdtfGlobal:
        assert(msGlobalIdtfAddrs.find(idtf) == msGlobalIdtfAddrs.end());
        msGlobalIdtfAddrs[idtf] = addr;
        break;
    }
}
