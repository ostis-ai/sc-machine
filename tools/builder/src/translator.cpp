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
    tStringAddrMap::iterator it = msGlobalIdtfAddrs.find(ext);
    sc_addr fmt_addr;

    if (it != msGlobalIdtfAddrs.end())
        fmt_addr = it->second;
    else
    {
        String fmtStr = "format_" + ext;
        fmt_addr = sc_memory_node_new(sc_type_node_class | sc_type_const);
        sc_helper_set_system_identifier(fmt_addr, fmtStr.c_str(), fmtStr.size());
        msGlobalIdtfAddrs[fmtStr] = fmt_addr;
    }

    // try to find format relation
    sc_addr nrel_format_addr;
    String nrel_format_str = NREL_FORMAT_STR;
    it = msGlobalIdtfAddrs.find(nrel_format_str);
    if (it != msGlobalIdtfAddrs.end())
        nrel_format_addr = it->second;
    else
    {
        nrel_format_addr = sc_memory_node_new(sc_type_node_norole | sc_type_const);
        sc_helper_set_system_identifier(nrel_format_addr, nrel_format_str.c_str(), nrel_format_str.size());
        msGlobalIdtfAddrs[nrel_format_str] = nrel_format_addr;
    }

    // connect sc-link with format
    sc_addr arc_addr = sc_memory_arc_new(sc_type_arc_common | sc_type_const, addr, fmt_addr);
    sc_memory_arc_new(sc_type_arc_pos_const_perm, nrel_format_addr, arc_addr);
}
