/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010-2014 OSTIS

OSTIS is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OSTIS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS.  If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
*/

#include "translator.h"

#define NREL_FORMAT_STR     "nrel_format"

iTranslator::tStringAddrMap iTranslator::msGlobalIdtfAddrs = iTranslator::tStringAddrMap();

iTranslator::iTranslator()
{
}

iTranslator::~iTranslator()
{
}

bool iTranslator::translate(const TranslatorParams &params)
{
    mParams = params;
    translateImpl();
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
        if (sc_helper_find_element_by_system_identifier(fmtStr.c_str(), fmtStr.size(), &fmt_addr) != SC_RESULT_OK)
        {
            fmt_addr = sc_memory_node_new(sc_type_node_class | sc_type_const);
            sc_helper_set_system_identifier(fmt_addr, fmtStr.c_str(), fmtStr.size());
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
        if (sc_helper_find_element_by_system_identifier(nrel_format_str.c_str(), nrel_format_str.size(), &nrel_format_addr) != SC_RESULT_OK)
        {
            nrel_format_addr = sc_memory_node_new(sc_type_node_norole | sc_type_const);
            sc_helper_set_system_identifier(nrel_format_addr, nrel_format_str.c_str(), nrel_format_str.size());
            mSysIdtfAddrs[nrel_format_str] = nrel_format_addr;
        }
    }

    // connect sc-link with format
    sc_addr arc_addr = sc_memory_arc_new(sc_type_arc_common | sc_type_const, addr, fmt_addr);
    sc_memory_arc_new(sc_type_arc_pos_const_perm, nrel_format_addr, arc_addr);
}
