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

Sc2SCgJsonTranslator::Sc2SCgJsonTranslator()
{

}

Sc2SCgJsonTranslator::~Sc2SCgJsonTranslator()
{

}

void Sc2SCgJsonTranslator::translate(const sc_addr &input_addr, const sc_addr &format_addr)
{
    mInputConstructionAddr = input_addr;
    mOutputFormatAddr = format_addr;

    mOutputJson = "[";
    bool first = true;

    sc_iterator3 *it = sc_iterator3_f_a_a_new(mInputConstructionAddr, sc_type_arc_pos_const_perm, 0);
    while (sc_iterator3_next(it) == SC_TRUE)
    {
        sc_type el_type = 0;
        sc_addr addr = sc_iterator3_value(it, 2);

        //! TODO add error logging
        if (sc_memory_get_element_type(addr, &el_type) != SC_RESULT_OK)
            continue;

        tStringStringMap attrs;

        String sys_idtf;
        bool idtf_exist = ui_translate_resolve_system_identifier(addr, sys_idtf);

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
            mOutputJson += ", ";

        mOutputJson += "{ ";
        tStringStringMap::iterator it, itEnd = attrs.end();
        for (it = attrs.begin(); it != itEnd; ++it)
        {
            if (it != attrs.begin())
                mOutputJson += ", ";
            mOutputJson += "\"" + it->first + "\": ";
            mOutputJson += "\"" + it->second + "\"";
        }
        mOutputJson += " }";

        first = false;


    }
    sc_iterator3_free(it);

    mOutputJson += "]";

    qDebug() << mOutputJson.c_str();

    // write into sc-link
    sc_stream *result_data_stream = 0;
    result_data_stream = sc_stream_memory_new(mOutputJson.c_str(), mOutputJson.size(), SC_STREAM_READ, SC_FALSE);

    sc_addr result_addr = sc_memory_link_new();
    sc_memory_set_link_content(result_addr, result_data_stream);

    sc_stream_free(result_data_stream);

    // generate translation
    sc_addr arc_addr = sc_memory_arc_new(sc_type_arc_common | sc_type_const, mInputConstructionAddr, result_addr);
    sc_memory_arc_new(sc_type_arc_pos_const_perm, ui_keynode_nrel_translation, arc_addr);
}

String Sc2SCgJsonTranslator::buildId(const sc_addr &addr) const
{
    StringStream ss;
    ss << addr.seg << "_" << addr.offset;
    return ss.str();
}

// ------------------------------------------------------------------------------
sc_result Sc2SCgJsonTranslator::ui_translate_sc2scg_json(sc_event *event, sc_addr arg)
{
    sc_addr cmd_addr, input_addr, format_addr;

    if (sc_memory_get_arc_end(arg, &cmd_addr) != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    if (ui_translate_command_resolve_arguments(cmd_addr, &format_addr, &input_addr) != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    if (format_addr == ui_keynode_format_scg_json)
    {
        Sc2SCgJsonTranslator translator;
        translator.translate(input_addr, format_addr);
    }

    return SC_RESULT_OK;
}
