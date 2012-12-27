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
#include "uiSc2ScsTranslator.h"

#include "uiKeynodes.h"
#include "uiTranslators.h"

// --------------------
Sc2ScsTranslator::Sc2ScsTranslator()
{
    mTypeToConnector[sc_type_arc_common] = ">";
    mTypeToConnector[sc_type_arc_pos_const_perm] = "->";
    mTypeToConnector[sc_type_edge_common] = "<>";
    mTypeToConnector[sc_type_arc_access] = "..>";

    mTypeToConnector[sc_type_edge_common | sc_type_const] = "<=>";
    mTypeToConnector[sc_type_edge_common | sc_type_var] = "_<=>";
    mTypeToConnector[sc_type_arc_common | sc_type_const] = "=>";
    mTypeToConnector[sc_type_arc_common | sc_type_var] = "_=>";
    mTypeToConnector[sc_type_arc_access | sc_type_const | sc_type_arc_neg | sc_type_arc_perm] = "-|>";
    mTypeToConnector[sc_type_arc_access | sc_type_var | sc_type_arc_neg | sc_type_arc_perm] = "_-|>";
    mTypeToConnector[sc_type_arc_access | sc_type_const | sc_type_arc_fuz | sc_type_arc_perm] = "-/>";
    mTypeToConnector[sc_type_arc_access | sc_type_var | sc_type_arc_fuz | sc_type_arc_perm] = "_-/>";
    mTypeToConnector[sc_type_arc_access | sc_type_const | sc_type_arc_pos | sc_type_arc_temp] = "~>";
    mTypeToConnector[sc_type_arc_access | sc_type_var | sc_type_arc_pos | sc_type_arc_temp] = "_~>";
    mTypeToConnector[sc_type_arc_access | sc_type_const | sc_type_arc_neg | sc_type_arc_temp] = "~|>";
    mTypeToConnector[sc_type_arc_access | sc_type_var | sc_type_arc_neg | sc_type_arc_temp] = "_~|>";
    mTypeToConnector[sc_type_arc_access | sc_type_const | sc_type_arc_fuz | sc_type_arc_temp] = "~/>";
    mTypeToConnector[sc_type_arc_access | sc_type_var | sc_type_arc_fuz | sc_type_arc_temp] = "_~/>";
}

Sc2ScsTranslator::~Sc2ScsTranslator()
{

}

void Sc2ScsTranslator::translate(const sc_addr &input_addr, const sc_addr &format_addr)
{
    mInputConstructionAddr = input_addr;
    mOutputFormatAddr = format_addr;

    collectObjects();

    run();

    // write into sc-link
    sc_stream *result_data_stream = 0;
    result_data_stream = sc_stream_memory_new(mOutputData.c_str(), mOutputData.size(), SC_STREAM_READ, SC_FALSE);

    sc_addr result_addr = sc_memory_link_new();
    sc_memory_set_link_content(result_addr, result_data_stream);

    sc_stream_free(result_data_stream);

    // generate translation
    sc_addr arc_addr = sc_memory_arc_new(sc_type_arc_common | sc_type_const, mInputConstructionAddr, result_addr);
    sc_memory_arc_new(sc_type_arc_pos_const_perm, ui_keynode_nrel_translation, arc_addr);
}

void Sc2ScsTranslator::collectObjects()
{
    sc_iterator3 *it = sc_iterator3_f_a_a_new(mInputConstructionAddr, sc_type_arc_pos_const_perm, 0);
    while (sc_iterator3_next(it) == SC_TRUE)
    {
        sc_type el_type = 0;
        sc_addr addr = sc_iterator3_value(it, 2);

        //! TODO add error logging
        if (sc_memory_get_element_type(addr, &el_type) != SC_RESULT_OK)
            continue;

        if (el_type & sc_type_node)
        {
            mNodes[addr] = el_type;
            continue;
        }

        if (el_type & sc_type_arc_mask)
        {
            mArcs[addr] = el_type;
            continue;
        }

        if (el_type & sc_type_link)
        {
            mLinks[addr] = el_type;
            continue;
        }

    }
    sc_iterator3_free(it);
}

void Sc2ScsTranslator::run()
{
    //! TODO logging sc-element, that can't be translated

    // iterate all arcs and translate them
    tScAddrToScTypeMap::iterator it, itEnd = mArcs.end();

    for (it = mArcs.begin(); it != itEnd; ++it)
    {
        const sc_addr &arc_addr = it->first;
        sc_type arc_type = it->second;

        sc_addr arc_beg, arc_end;
        // get begin and end arc elements
        if (sc_memory_get_arc_begin(arc_addr, &arc_beg) != SC_RESULT_OK)
            continue; //! TODO logging

        if (isNeedToTranslate(arc_beg) == false)
            continue; //! TODO logging

        if (sc_memory_get_arc_end(arc_addr, &arc_end) != SC_RESULT_OK)
            continue; //! TODO logging

        if (isNeedToTranslate(arc_end) == false)
            continue; //! TODO logging

        String begin_idtf, end_idtf;
        resolveSystemIdentifier(arc_beg, begin_idtf);
        resolveSystemIdentifier(arc_end, end_idtf);

        // determine arc type
        String arc_connector = "<>";
        tScTypeToSCsConnectorMap::const_iterator it = mTypeToConnector.find(arc_type);
        if (it != mTypeToConnector.end())
            arc_connector = it->second;

        mOutputData += begin_idtf + " " + arc_connector + " " + end_idtf + ";;\n";
    }

}

void Sc2ScsTranslator::resolveSystemIdentifier(const sc_addr &addr, String &idtf)
{
    tSystemIdentifiersMap::iterator it = mSystemIdentifiers.find(addr);
    if (it != mSystemIdentifiers.end())
    {
        idtf = it->second;
        return;
    }

    ui_translate_resolve_system_identifier(addr, idtf);
    mSystemIdentifiers[addr] = idtf;
}

bool Sc2ScsTranslator::isNeedToTranslate(const sc_addr &addr) const
{
    if (mNodes.find(addr) != mNodes.end())
        return true;

    if (mArcs.find(addr) != mArcs.end())
        return true;

    if (mLinks.find(addr) != mLinks.end())
        return true;

    return false;
}

// -------------------------------------------------------
sc_result Sc2ScsTranslator::ui_translate_sc2scs(sc_event *event, sc_addr arg)
{
    sc_addr cmd_addr, input_addr, format_addr;

    if (sc_memory_get_arc_end(arg, &cmd_addr) != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    if (ui_translate_command_resolve_arguments(cmd_addr, &format_addr, &input_addr) != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    if (format_addr == ui_keynode_format_scs)
    {
        Sc2ScsTranslator translator;
        translator.translate(input_addr, format_addr);
    }

    return SC_RESULT_OK;
}
