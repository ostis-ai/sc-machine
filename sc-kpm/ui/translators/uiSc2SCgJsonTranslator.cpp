/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "uiPrecompiled.h"
#include "uiSc2SCgJsonTranslator.h"

#include "uiKeynodes.h"
#include "uiTranslators.h"
#include "uiUtils.h"

uiSc2SCgJsonTranslator::uiSc2SCgJsonTranslator()
{
}

uiSc2SCgJsonTranslator::~uiSc2SCgJsonTranslator()
{

}

void uiSc2SCgJsonTranslator::runImpl()
{
    bool first = true;
    sc_type el_type = 0;
    sc_addr addr;
    tStringStringMap attrs;

    mOutputData = "[";

    sc_iterator3 *it = sc_iterator3_f_a_a_new(s_default_ctx, mInputConstructionAddr, sc_type_arc_pos_const_perm, 0);
    while (sc_iterator3_next(it) == SC_TRUE)
    {
        el_type = 0;
        addr = sc_iterator3_value(it, 2);

        //! TODO add error logging
        if (sc_memory_get_element_type(s_default_ctx, addr, &el_type) != SC_RESULT_OK)
            continue;

        attrs.clear();

        attrs["id"] = buildId(addr);
        StringStream ss;
        ss << el_type;
        attrs["el_type"] = ss.str();
        if (el_type & sc_type_node)
            attrs["type"] = "node";

        if (el_type & sc_type_arc_mask)
        {
            attrs["type"] = "arc";

            sc_addr beg_addr;
            if (sc_memory_get_arc_begin(s_default_ctx, addr, &beg_addr) != SC_RESULT_OK)
                continue; //! TODO error logging
            sc_addr end_addr;
            if (sc_memory_get_arc_end(s_default_ctx, addr, &end_addr) != SC_RESULT_OK)
                continue; //! TODO error logging

            attrs["begin"] = buildId(beg_addr);
            attrs["end"] = buildId(end_addr);
        }

        if (el_type & sc_type_link)
        {
            attrs["type"] = "link";
        }

        if (!first)
            mOutputData += ", ";

        mOutputData += "{ ";
        tStringStringMap::iterator itAttrs, itAttrsEnd = attrs.end();
        for (itAttrs = attrs.begin(); itAttrs != itAttrsEnd; ++itAttrs)
        {
            if (itAttrs != attrs.begin())
                mOutputData += ", ";
            mOutputData += "\"" + itAttrs->first + "\": ";
            mOutputData += "\"" + itAttrs->second + "\"";
        }
        mOutputData += " }";

        first = false;
    }
    sc_iterator3_free(it);

    mOutputData += "]";
}

// ------------------------------------------------------------------------------
sc_result uiSc2SCgJsonTranslator::ui_translate_sc2scg_json(const sc_event *event, sc_addr arg)
{
    sc_addr cmd_addr, input_addr, format_addr;

    if (sc_memory_get_arc_end(s_default_ctx, arg, &cmd_addr) != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    if (ui_check_cmd_type(cmd_addr, keynode_command_translate_from_sc) != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    if (ui_translate_command_resolve_arguments(cmd_addr, &format_addr, &input_addr) != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    if (format_addr == keynode_format_scg_json)
    {
        uiSc2SCgJsonTranslator translator;
        translator.translate(input_addr, format_addr);
    }

    return SC_RESULT_OK;
}
