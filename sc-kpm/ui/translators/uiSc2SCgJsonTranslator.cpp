/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2012 OSTIS

OSTIS is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
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

#include "uiPrecompiled.h"
#include "uiSc2SCgJsonTranslator.h"

#include "uiKeynodes.h"
#include "uiTranslators.h"

#include <QDebug>

uiSc2SCgJsonTranslator::uiSc2SCgJsonTranslator()
{

}

uiSc2SCgJsonTranslator::~uiSc2SCgJsonTranslator()
{

}

void uiSc2SCgJsonTranslator::runImpl()
{
    bool first = true;
    bool idtf_exist = false;
    sc_type el_type = 0;
    sc_addr addr;
    String sys_idtf;
    tStringStringMap attrs;

    mOutputData = "[";

    sc_iterator3 *it = sc_iterator3_f_a_a_new(mInputConstructionAddr, sc_type_arc_pos_const_perm, 0);
    while (sc_iterator3_next(it) == SC_TRUE)
    {
        el_type = 0;
        addr = sc_iterator3_value(it, 2);

        //! TODO add error logging
        if (sc_memory_get_element_type(addr, &el_type) != SC_RESULT_OK)
            continue;

        attrs.clear();

        idtf_exist = ui_translate_resolve_system_identifier(addr, sys_idtf);

        attrs["id"] = buildId(addr);
        if (idtf_exist)
            attrs["identifier"] = sys_idtf;
        if (el_type & sc_type_node)
        {
            attrs["type"] = "node";
            attrs["el_type"] = "node/const/general_node";
        }

        if (el_type & sc_type_arc_mask)
        {
            attrs["type"] = "arc";

            sc_addr beg_addr;
            if (sc_memory_get_arc_begin(addr, &beg_addr) != SC_RESULT_OK)
                continue; //! TODO error logging
            sc_addr end_addr;
            if (sc_memory_get_arc_end(addr, &end_addr) != SC_RESULT_OK)
                continue; //! TODO error logging

            attrs["begin"] = buildId(beg_addr);
            attrs["end"] = buildId(end_addr);
            attrs["el_type"] = "arc/const/pos";
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
sc_result uiSc2SCgJsonTranslator::ui_translate_sc2scg_json(sc_event *event, sc_addr arg)
{
    sc_addr cmd_addr, input_addr, format_addr;

    if (sc_memory_get_arc_end(arg, &cmd_addr) != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    if (ui_translate_command_resolve_arguments(cmd_addr, &format_addr, &input_addr) != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    if (format_addr == ui_keynode_format_scg_json)
    {
        uiSc2SCgJsonTranslator translator;
        translator.translate(input_addr, format_addr);
    }

    return SC_RESULT_OK;
}
